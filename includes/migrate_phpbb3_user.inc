<?php

class MigratePhpbb3UserMigration extends MigratePhpbb3Migration {

  public function __construct() {
    parent::__construct();
    $this->description = t('PHPBB 3 Users');
    
  }
}