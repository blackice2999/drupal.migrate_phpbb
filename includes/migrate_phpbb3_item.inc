<?php
/**
 * Created by JetBrains PhpStorm.
 * User: blackice2999
 * Date: 09.08.11
 * Time: 23:42
 * To change this template use File | Settings | File Templates.
 */
 
abstract class MigratePhpbb3ItemMigration extends MigratePhpbb3Migration {
  public function __construct() {
    parent::__construct();
   }

  public function prepareRow($row) {
    $debug = '';

    // remove phpbb3 extra bbcode from content
    if (isset($row->post_text)) {

      // remove the bbcode_uid from post_text
      if (!empty($row->bbcode_uid)) {
        $row->post_text = preg_replace("/:$row->bbcode_uid/", '', $row->post_text);
      }

      // remove the phpbb3 extra bbcode from text
      $row->post_text = $this->_stripExtraBBCode($row->post_text);
    }
  }

  protected function _stripExtraBBCode($text = '') {
    // Strip the text of extra markup - regular expressions taken from phpbb3 includes/function.php.
    $match = array(
      '#<!\-\- e \-\-><a href="mailto:(.*?)">.*?</a><!\-\- e \-\->#',
      '#<!\-\- l \-\-><a (?:class="[\w-]+" )?href="(.*?)(?:(&amp;|\?)sid=[0-9a-f]{32})?">.*?</a><!\-\- l \-\->#',
      '#<!\-\- ([mw]) \-\-><a (?:class="[\w-]+" )?href="(.*?)">.*?</a><!\-\- \1 \-\->#',
      '#<!\-\- s(.*?) \-\-><img src="\{SMILIES_PATH\}\/.*? \/><!\-\- s\1 \-\->#',
      '#<!\-\- .*? \-\->#s',
      '#<.*?>#s',
    );
    $replace = array('$1', '$1', '$2', '$1', '', '');
    $text = preg_replace($match, $replace, $text);

    return $text;
  }
}
