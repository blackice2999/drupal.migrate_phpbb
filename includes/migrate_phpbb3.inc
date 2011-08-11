<?php

/**
 *
 */
abstract class MigratePhpbb3Migration extends Migration {

  public function __construct() {
    parent::__construct();

    // good point to init the database ?
    $phpbb3_database = array(
      'database' => 'devTriOptimumPhpBB',
      'username' => 'webdev',
      'password' => 'webdev',
      'host' => 'localhost',
      'driver' => 'mysql',
      'prefix' => 'phpbb_',
    );

    Database::addConnectionInfo('phpbb', 'default', $phpbb3_database);
  }

}