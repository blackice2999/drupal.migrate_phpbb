<?php


class MigratePhpbb3TopicMigration extends MigratePhpbb3ItemMigration {

  public function __construct() {
    parent::__construct();

    $this->description = t('PHPBB3 Topics');

    $this->map = new MigrateSQLMap($this->machineName,
      array('topic_id' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      )),
      MigrateDestinationNode::getKeySchema()
    );

    $query = Database::getConnection('default', 'phpbb')
      ->select('topics', 'pt')
      ->fields('pt', array(
                          'topic_poster',
                          'topic_type',
                          'topic_status',
                          'topic_time',
                          'topic_title',
                          'topic_moved_id', // ???
                          'topic_id',
                          'forum_id',
                     ))
      ->fields('pp', array(
                          'bbcode_uid',
                          'post_text',
                          'post_edit_time',
                          'post_id',
                     ));

    $query->innerJoin('posts', 'pp', 'pp.post_id = pt.topic_first_post_id');

    $this->source = new MigrateSourceSQL($query, array(), NULL, array('map_joinable' => FALSE));

    $this->destination = new MigrateDestinationNode('forum', array('text_format' => 'plain_text'));


    $this->addFieldMapping('title', 'topic_title');

    // @TODO: add sourcemapping to user...
    $this->addFieldMapping('uid', 'topic_poster')
      ->sourceMigration('MigratePhpbb3User')
      ->defaultValue(0);

    $this->addFieldMapping('language', NULL)
      ->defaultValue(LANGUAGE_NONE);

    $this->addFieldMapping('created', 'topic_time');

    $this->addFieldMapping('is_new', NULL)
      ->defaultValue(TRUE);

    $this->addFieldMapping('changed', 'post_edit_time');

    $this->addFieldMapping('status')
      ->defaultValue(NODE_PUBLISHED);

    $this->addFieldMapping('promote')
      ->defaultValue(NODE_NOT_PROMOTED);

    $this->addFieldMapping('sticky', 'topic_type')
      ->defaultValue(NODE_NOT_STICKY);

    $this->addFieldMapping('comment')
      ->defaultValue(COMMENT_NODE_OPEN);
    
    $this->addFieldMapping('taxonomy_forums', 'forum_id')
      ->sourceMigration('MigratePhpbb3Categories')
      ->arguments(array('source_type' => 'tid'));

    $arguments = array('source_field' => 'excerpt', 'format' => 'user_inputs');
    $this->addFieldMapping('body', 'post_text')
         ->arguments($arguments);
  }

  public function prepareRow($row) {
    // if we dont have a change time use the create time
    // @TODO: on posts we need the time of last post.
    if ($row->post_edit_time == "0" || !(isset($row->post_edit_time))) {
      $row->post_edit_time = $row->topic_time;
    }

    // anonymous user is on phpbb -1 change to 0
    if ($row->topic_poster == -1) {
      $row->topic_poster = 0;
    }

    parent::prepareRow($row);
  }

  public function prepare($node, stdClass $row){
    // if nid is not set we have new content
    // we need to inject a NID because of core bug in
    // forum module. see: http://drupal.org/node/839770
    if (!isset($node->nid)) {
      $node->nid = NULL;
    }
  }
}