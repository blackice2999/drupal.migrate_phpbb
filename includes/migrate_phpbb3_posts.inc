<?php

class MigratePhpbb3PostsMigration extends MigratePhpbb3ItemMigration
{

  public function __construct()
  {
    parent::__construct();

    $this->dependencies = array('MigratePhpbb3Categories', 'MigratePhpbb3User', 'MigratePhpbb3Topic');

    $this->description = t('PHPBB3 Posts');

    $this->map = new MigrateSQLMap($this->machineName,
      array('post_id' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      )),
      MigrateDestinationComment::getKeySchema()
    );

    $query = Database::getConnection('default', 'phpbb')
      ->select('posts', 'pp')
      ->fields('pp', array(
                          'poster_id',
                          'topic_id',
                          'bbcode_uid',
                          'post_text',
                          'post_subject',
                          'poster_ip',
                          'post_time',
                          'post_edit_time',
                          'post_approved',
                          'post_id',
                     ));
    $query->condition('topic_id', 'post_id', '<>');

    $this->source = new MigrateSourceSQL($query, array(), NULL, array('map_joinable' => FALSE));
    $this->destination = new MigrateDestinationComment('comment_node_forum');

    $this->addFieldMapping('nid', 'topic_id')
      ->sourceMigration('MigratePhpbb3Topic');

    $this->addFieldMapping('uid', 'poster_id')
      ->sourceMigration('MigratePhpbb3User')
      ->defaultValue(0);

    $this->addFieldMapping('subject', 'post_subject');

    $this->addFieldMapping('status', 'post_approved')
      ->defaultValue(COMMENT_PUBLISHED);

    $this->addFieldMapping('comment_body', 'post_text');
    $this->addFieldMapping('created', 'post_time');
    $this->addFieldMapping('changed', 'post_edit_time');

    // will be filled in prepare
    $this->addFieldMapping('name');

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('mail','homepage','hostname', 'thread', 'language'));
  }

  public function prepareRow($row) {
    // if we dont have a change time use the create time
    // @TODO: on posts we need the time of last post.
    if ($row->post_edit_time == "0" || !(isset($row->post_edit_time))) {
      $row->post_edit_time = $row->post_time;
    }

    // anonymous user is on phpbb -1 change to 0
    if ($row->poster_id == -1) {
      $row->poster_id = 0;
    }

    $row->post_subject = substr($row->post_subject, 0, 64);

    parent::prepareRow($row);
  }

  public function prepare($comment, stdClass $row)
  {
    $comment_user = user_load($comment->uid);
    $comment->name = $comment_user->name;
  }

}