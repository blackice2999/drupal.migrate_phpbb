<?php
class MigratePhpbb3CategoriesMigration extends MigratePhpbb3Migration {

  private $addedContainers = array();
  private $deletedContainers = array();

  /**
   * contains the forum vocabulary. Will be set on __construct()
   * @access private
   * @var null|string
   */
  private $forumVocabulary = NULL;

  public function __construct() {
    parent::__construct();

    // get forum vocabulary id and store vocabulary object
    $this->forumVocabulary = taxonomy_vocabulary_load( variable_get('forum_nav_vocabulary', 0) );

    $this->description = t('PHPBB3 Categories');

    $this->map = new MigrateSQLMap($this->machineName,
      array('forum_id' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      )),
      MigrateDestinationTerm::getKeySchema()
    );
    
    $query = Database::getConnection('default', 'phpbb')
      ->select('forums', 'phpbb')
      ->fields('phpbb', array(
                             'forum_id',
                             'parent_id',
                             'forum_name',
                             'forum_desc',
                             'forum_type',
                        ));

    $this->source = new MigrateSourceSQL($query, array(), NULL, array('map_joinable' => FALSE));

    $this->destination = new MigrateDestinationTerm($this->forumVocabulary->machine_name);
    $this->addFieldMapping('name', 'forum_name');
    $this->addFieldMapping('description', 'forum_desc');
    $this->addFieldMapping('parent', 'parent_id')
      ->sourceMigration($this->getMachineName());
  }


  public function prepareRow($row) {
    // if parent_id is 0 we need to set it to NULL or migrate try to find a forum_id with 0
    if ($row->parent_id == 0) {
      $row->parent_id = NULL;
    }
  }

  /**
   * creates stub terms to ensure that child always can get a parent
   * if child is created before parent
   * @return array|tid
   */
  protected function createStub() {

    // create a empty term
    $term = new stdClass;
    $term->vocabulary_machine_name = $this->forumVocabulary->machine_name;
    $term->name = t('Stub Term');
    $term->description = t('Placeholder Stub Term');

    $status = taxonomy_term_save($term);
    if ($status == SAVED_NEW) {
      return array($term->tid);
    }
    else {
      return false;
    }
  }

  /**
   * forum containers...
   * @param $entity
   * @param stdClass $row
   * @return void
   */
  public function complete($entity, stdClass $row) {
    if (!$entity->parent || !$entity->parent[0]) {
      $this->addedContainers[] = $entity->tid;
    }
  }

  /**
   * add forum containers to variable
   * @return void
   */
  public function postImport() {
    $forum_containers = variable_get('forum_containers', array());

    foreach($this->addedContainers as $container){
      // check if term already exists - if not add them
      if (!in_array($container, $forum_containers)) {
        $forum_containers[] = $container;
      }
    }
    variable_set('forum_containers', $forum_containers);
  }

  /**
   * deleted containers...
   * @param $entity_id
   * @return void
   */
  public function completeRollback($entity_id) {
    $this->deletedContainers[] = $entity_id;
  }

  /**
   * remove container from forum
   * @return void
   */
  public function postRollback() {
    $containers = variable_get('forum_containers', array());
    $remaining = array();
    foreach ($containers as $container) {
      if (!in_array($container, $this->deletedContainers) && !in_array($container, $remaining)) {
        $remaining[] = $container;
      }
    }
    variable_set('forum_containers', $remaining);
  }
}