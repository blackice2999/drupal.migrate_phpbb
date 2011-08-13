<?php
/**
 * Created by JetBrains PhpStorm.
 * User: blackice2999
 * Date: 09.08.11
 * Time: 23:42
 * To change this template use File | Settings | File Templates.
 */

abstract class MigratePhpbb3ItemMigration extends MigratePhpbb3Migration
{
  public function __construct()
  {
    parent::__construct();

    $this->dependencies = array('MigratePhpbb3Categories', 'MigratePhpbb3User');
  }

  public function prepareRow($row)
  {
    $debug = '';

    // remove phpbb3 extra bbcode from content
    if (isset($row->post_text)) {
      // remove the bbcode_uid from post_text
      if (!empty($row->bbcode_uid)) {
        $row->post_text = $this->_removeBBCodeUID($row->post_text, $row->bbcode_uid);
      }

      // remove the phpbb3 extra bbcode from text
      $row->post_text = $this->_stripExtraBBCode($row->post_text);
    }
  }

}
