<?php

/**
 *
 */
abstract class MigratePhpbb3Migration extends Migration
{

  public function __construct()
  {
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

  /**
   * PHPBB3 uses a UID on bbcode for xss protecting ?! - we need to remove it...
   * @param string $text
   * @param null $bbcode_uid
   * @return mixed
   */
  protected function _removeBBCodeUID($text = '', $bbcode_uid = NULL){
     return preg_replace("/:$bbcode_uid/", '', $text);
  }

  /**
   * PHPBB3 uses in signatures / topics & posts some special bbcode
   * this method parse them and removes
   * @param string $text
   * @return mixed|string
   */
  protected function _stripExtraBBCode($text = '')
  {
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