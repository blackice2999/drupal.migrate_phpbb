<?php

class MigratePhpbb3UserMigration extends MigratePhpbb3Migration
{

  /**
   * The Drupal UID of the new PHPBB Admin
   * @var int
   */
  protected $phpbb_admin = 1;

  public function __construct()
  {
    parent::__construct();


    $this->description = t('PHPBB 3 Users');

    $this->map = new MigrateSQLMap($this->machineName,
      array('user_id' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      )),
      MigrateDestinationUser::getKeySchema()
    );

    $query = Database::getConnection('default', 'phpbb')
      ->select('users', 'pu')
      ->fields('pu', array(
                          'user_id',
                          'user_type',
                          'user_regdate',
                          'username',
                          'username_clean',
                          'user_password',
                          'user_email',
                          'user_birthday',
                          'user_lastvisit',
                          'user_posts',
                          'user_lang',
                          'user_from',
                          'user_icq',
                          'user_aim',
                          'user_yim',
                          'user_msnm',
                          'user_jabber',
                          'user_website',
                          'user_sig',
                          'user_sig_bbcode_uid'
                     ));

    // exclude users with no posts written...
    // exclude unreal users (usertype 2)
    $query->condition('user_posts', 0, '>')->condition('user_type', 2, '!=');

    $this->source = new MigrateSourceSQL($query, array(), NULL, array('map_joinable' => FALSE));

    $this->destination = new MigrateDestinationUser();

    $this->addFieldMapping('mail', 'user_email');
    $this->addFieldMapping('init', 'user_email')
      ->defaultValue('');

    $this->addFieldMapping('name', 'username_clean')
      ->dedupe('users', 'name'); // ensure unique names

    $this->addFieldMapping('language', 'user_lang')
      ->defaultValue('en');

    // @TODO: move it into a seperate field...
    $this->addFieldMapping('signature', 'user_sig');

    $this->addFieldMapping('created', 'user_regdate');
    $this->addFieldMapping('access', 'user_lastvisit');
    $this->addFieldMapping('login', 'user_lastvisit');


    $this->addFieldMapping('status')
      ->defaultValue(TRUE);

    // Ensure that we have a new password for the user, because we cant import the existing password hash
    // into drupal... @TODO: check if we can inject the password hash via complete() method...
    $this->addFieldMapping('pass')
      ->defaultValue(user_password());
  }

  public  function prepareRow($row) {
    // remove extra bbcode from signature
    if (isset($row->user_sig)) {
      if (!empty($row->user_sig_bbcode_uid)) {
        $row->user_sig = $this->_removeBBCodeUID($row->user_sig, $row->user_sig_bbcode_uid);
      }
      // remove phpbb extra bbcode from sig
      $row->user_sig = $this->_stripExtraBBCode($row->user_sig);
    }

    $row->user_sig = substr($row->user_sig, 0, 254);
  }

  public function prepare($account, stdClass $row)
  {
    // if nid is not set we have new content
    // we need to inject a NID because of core bug in
    // forum module. see: http://drupal.org/node/839770
    if (!isset($account->uid)) {
      $account->uid = NULL;
    }
  }
}