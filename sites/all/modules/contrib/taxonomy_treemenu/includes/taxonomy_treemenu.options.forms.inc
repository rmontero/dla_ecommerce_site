<?php
// $Id: taxonomy_treemenu.options.forms.inc,v 1.8 2009/10/01 10:51:30 rcrowther Exp $

// All this is in one file for now, but the classes could be broken into
// seperate files, should the form or the rendering side grow unwieldy.



class TTMOptsQuery extends TTMOptsDefaults
{

  /**
   * Drop inactive options and convert some values to be sql friendly.
   *
   * @param <type> $opts
   * @return <type>
   */
  static private function admin2SQL($opts) {
    //dpm('adminSQL');
    $filtered = array();
    
    // Drop all inactive values.
    $inactive = create_function('$opt', 'return $opt["active"];');
    $filtered['sort'] = array_filter($opts['sort'],  $inactive);
    $filtered['filter'] = array_filter($opts['filter'], $inactive);
    $filtered['fields'] = array_filter($opts['fields'], $inactive);


    // Delve into the sub opts
    foreach ($opts as $gid => &$group ) {
      foreach ($group as $oid => &$opt) {
        if (is_array($opt['value'])) {
          $filtered[$gid][$oid]['value'] = array_filter($opt['value']);
        }
      }
    }

    // Lower should be more significant, I think.
    // So we need to array reverse weighed groups. With our construction, this is easy.
    // We must identify by name, though, as we've shed the info fields.
    $filtered['sort'] = array_reverse($filtered['sort'], TRUE);

    // swap toggle values in sort for the more SQL friendly 'ASC' 'DESC'
    foreach($filtered['sort'] as &$opt) {
      $opt['value'] = ($opt['value']) ? 'ASC' : 'DESC';
    }
    //dpm('filter:');
    //dpm($filtered);
    return $filtered;
  }


  /**
   * Tag builder for orderers. Doesn't use placeholders, as nothing is user
   * input.
   * @param <type> $order_opts
   * @return <type>
   */
  static private function order_tag($order_opts) {
    $tag = array();
    foreach ($order_opts as $data) {
      $tag[] = ' n.'. $data['field'] .' '. $data['value'];
    }
    return implode(',', $tag);
  }


  static private function placeholder($value) {
    return (is_numeric($value)) ? '%d' : "'%s'";
  }


  /**
   * Generic tag builder using placeholders.
   * Detects the placeholder type from the value it finds.
   *
   * @param <type> $args
   *   Pass in a reference to pile up the args.
   * @param <type> $group
   * @param <type> $separator
   *  SQL, e.g. ' AND ', ', '
   * @param <type> $prefix
   *  Add the separator to the start of the return.
   * @return <type>
   *  A string of placeholders, imploded with the separator.
   */
  // TODO: for now, this always ORs a sub opt. Is that enough?
  static private function tag(&$args, $group, $separator, $prefix = FALSE) {
    $phs = array();
    foreach ($group as $data) {
      $item = '';
      // Sub options are 'OR'ed in the query.
      if (is_array($data['value'])) {
          $sub_phs = array();
          foreach ($data['value'] as $field => $sub_data) {
                $sub_phs[] = ' n.'. $data['field'] .' = '. self::placeholder($field) .' ';
                $args[] = $field;
          }
          $item = '('. implode(' OR ', $sub_phs) .')';
      }
      else {
          $item = ' n.'. $data['field'] .' = '. self::placeholder($data['value']);
          $args[] = $data['value'];
      }
      $phs[] = $item;

    }

    //dvm($phs);
    $query = implode($separator, $phs);
    if ($query && $prefix) {
      $query = $separator . $query;
    }
    return $query;
  }


  //TODO: sticky/title/created is the stock return, by why not more? Especially for terms?
  /**
   * Return an array of SQL data for a db query.
   * Adjusted for language and TTM options.
   * @param $ttm
   * @param $tids
   *   A simple array of tid values.
   * @param $i18n_selection_mode
   *   Triggered if i18n is exists. defaults to 'off' mode.
   * @return
   *   An array containing keys 'query' 'count'(query) and 'args' for SQL data.
   */
  static function nodes($ttm, $tids, $i18n_selection_mode = 'default') {
    // Cache in a static, as sequential calls only change the tid. Also good for
    // repeated menus on a page.
    //dpm('::nodes');
    //dvm($tids);
    //static $query;
    //dpm('SQL ops');

    //if (!$query[$ttm['menu_name']]) {
      //dvm($opts);
      $sql  = array();
      $args = array();
      $opts = unserialize($ttm['options']);
      // if opts is null return a default?
      $opts = self::admin2SQL($opts);
      //dpm($ttm['menu_name']);
      //dpm($opts);
      // Sort. Always somthing here - see formProcess() in TTMOptsAdmin
      $order_tag = self::order_tag($opts['sort']);

      // Add the tid (rename for the sake of clarity).
      //$sql['tid_pos'] = count($args);
      //$args[] = $tid;
      $args = $tids;
      $placeholders = db_placeholders($args);

      // Filter tag.
      if (!empty($opts['filter'])) {
        $filter_tag = self::tag($args, $opts['filter'], ' AND ', TRUE);
      }

    
      //dpm($args);
      //dvm($filter_ph);

      $fields_tag =  ', n.sticky, n.title, n.created ';
      //$language_tag = 'n.language ="en" OR n.language = "" OR n.language IS NULL';
      $sql['args'] = $args;
// WHERE (n.language ='en' OR n.language ='' OR n.language IS NULL) AND
      //$sql['query'] = db_rewrite_sql('SELECT DISTINCT(n.nid) '. $fields_tag .' FROM {node} n INNER JOIN {term_node} tn ON n.nid = tn.nid WHERE tn.tid = %d '. $filter_tag .' ORDER BY'. $order_tag);
      /*
      if (module_exists('i18n') &&  $default_language) {
        //dpm('i18n root lockout');
        i18n_selection_mode('default');
      }
      */


      //  if (module_exists('i18n')) i18n_selection_mode($i18n_selection_mode);
        if (module_exists('i18n')) taxonomy_treemenu_i18n_node_selection_mode($ttm, $i18n_selection_mode);

      $sql['query'] = db_rewrite_sql('SELECT DISTINCT(n.nid) '. $fields_tag .' FROM {node} n INNER JOIN {term_node} tn ON n.nid = tn.nid WHERE tn.tid IN ('. $placeholders .')'. $filter_tag .' ORDER BY'. $order_tag);
      //$sql['query'] = 'SELECT DISTINCT(n.nid) '. $fields_tag .' FROM {node} n INNER JOIN {term_node} tn ON n.nid = tn.nid WHERE ('.$language_tag .') AND (tn.tid IN ('. $placeholders .')'. $filter_tag .' ) ORDER BY'. $order_tag;
      //$sql['query'] = 'SELECT DISTINCT(n.nid), '. $fields_tag .' FROM {node} n INNER JOIN {term_node} tn ON n.vid = tn.vid WHERE tn.tid = %d '. $filter_tag .' ORDER BY'. $order_tag;
      //dpm($sql);
      //$sql['count'] = db_rewrite_sql('SELECT COUNT(DISTINCT(n.nid)) FROM {node} n INNER JOIN {term_node} tn ON n.nid = tn.nid WHERE tn.tid = %d '. $filter_tag);
      $sql['count'] = db_rewrite_sql('SELECT COUNT(DISTINCT(n.nid)) FROM {node} n INNER JOIN {term_node} tn ON n.nid = tn.nid WHERE tn.tid IN ('. $placeholders .')'. $filter_tag);
      //$sql['count'] = 'SELECT COUNT(DISTINCT(n.nid)) FROM {node} n INNER JOIN {term_node} tn ON n.nid = tn.nid WHERE ('.$language_tag .') AND (tn.tid IN ('. $placeholders .')'. $filter_tag .')';
      //$sql['count'] = 'SELECT COUNT(DISTINCT(n.nid)), FROM {node} n INNER JOIN {term_node} tn ON n.vid = tn.vid WHERE tn.tid = %d '. $filter_tag;
        if (module_exists('i18n')) i18n_selection_mode('reset');

      /*
      if (module_exists('i18n') &&  $default_language) {
         i18n_selection_mode('reset');
      }
      */
      //$query[$ttm['menu_name']] = $sql;
    //}
    //$query[$ttm['menu_name']]['args'][0] = $tid;
    //dpm($sql['query']);
    //dpm($sql['count']);
    //dpm($sql['args']);
    return $sql;
    //return $query[$ttm['menu_name']];
  }


  static function viewsArgs($opts) {
    return self::admin2SQL($opts);
  }

}

/**
 * Provides defaults for the menu options.
 * Also provides all the admin data for admin interfaces; option descriptions,
 * alternative titles and the like.
 * Caches the output in static variables, so rendering jobs, in particular,
 * don't have to lug a lot of code which they may not use.
 */
class TTMOptsDefaults
{
  /**
   * Option defaults and associated descriptive admin data. You can put static
   * stuff here. For dynamic information, this data is subsequently modified by
   * the class functions.
   *
   */
    
     // Don't go confusing this with a form data array, it's not. But it's a nice,
     // PHPish way of gathering data, so we use it. And it can be changed into
     // Drupal form elements very easily.
     //
     // There are differences. There are several abreviations, specialist elements
     // (#value becomes #default_value) #field, #child_value, and the builder constructs especially
     // for the tabs, making a child of toggles, but separating and indenting
     // sub-checkboxes.
     //
     // Some help,
     // The key for groups and options is the internal id.
     // The handlers are not recursive. They will put in a checkbox, then a child
     //   of some kind, though the child can be a stack of checkboxes, for example.
     // All options start with a checkbox (which is programmed in). This changes
     //   'active' in the stripped options.
     // If there is only one child, it must be a ttm_toggle (may seem limiting,
     //  but this makes much cleaner code. The builder in particular
     //  would have a problem here)
     //'#children' must be explicitly stated. It can be an array of sub boxes
     //  or a single lone 'ttm_toggle'tells treemenu about the children.
     // This will form the 'value' in stripped values, and later is basicly
     //  what goes into the SQL.
     // You do not need to explicity state that options are children of groups.
     // To resolve this, groups use the property # signage. For consistency we've
     //   ask for the # nested deeper, though it is not needed.
     // The key name for children should be the value which is placed in 'values'.
     //  The exception is a ttm_toggle, which knows what it has to do and should
     //  be keyed 'ttm_toggle' (ttm_toggles need a #value, for #default_value).
     //
     // All top level elements of options are turned into checkboxes. So you
     //  don't need to specify the #type.
     // Top level #values are set to 0, unless positivly specified as 1.
     // Note: 'field' is inherited, so need not be declared in sub-elements.
     // You can do without any children at all. The stripped option becomes
     //   active or not, and can handle true or false in 'active'.
     // The order of weighed arrays is mildly significant, as the field 'weight'
     //  is added programatically. i.e. the order here is the default order for
     //  display and sorting.

     protected static $options_fixed = array(
	    'filter' => array(
        '#title' => 'filter',
        'node_types' => array('#title' => 'Node Types',
              '#field' => 'type', '#value' => 1,
              ),
        'published' => array('#title' => 'Published', '#description'  => 'Wether published.',
              '#field' =>'status',
              '#value' => 1,
              '#children' => array(
                  'ttm_toggle'       => array(
                        '#type'        => 'ttm_toggle',
                        '#titles'       => array('YES', 'NO'),
                        '#value'       => 1,
                        ),
               ),
              ),
	    ),
      
	    'fields' => array(
        '#type'    => 'weighed',
        '#title' => 'fields',
        'uid'  => array('#title' => 'User', '#description'  => 'Name of user.',
              '#field' => 'uid', '#value' => 0),
       // 'status'  => array('#title' => 'Published', '#description'  => 'Indicate if published.',
       //       '#field' => 'status', '#value' => 0),
        'created'  => array('#title' => 'Created', '#description'  => 'Date of creation.',
              '#field' => 'created', '#value' => 0),
        'changed' => array('#title' => 'Changed', '#description'  => 'Date of most recent save.',
              '#field' => 'changed', '#value' => 0),
       // 'promote' => array('#title' => 'Promoted', '#description'  => 'Indicate if promoted to front page.',
       //       '#field' => 'promote', '#value' => 0),
       // 'sticky' => array('#title' => 'Sticky', '#description'  => 'Indicate if sticky.',
       //       '#field' => 'promote', '#value' => 0),
	    ),

	    'sort' => array(
        '#type'    => 'weighed',
        '#title' => 'sort',
        'title'   => array( '#title' => 'Title', '#description'   => 'Sort titles by alphabet.',
              '#field' =>'title', '#value' => 0,
              // For illustration...
              '#children' => array(
                    'ttm_toggle'       => array(
                          '#type'        => 'ttm_toggle',
                          '#titles'       => array('ASC', 'DESC'),
                          '#value'       => 1,
                          ),
                     ),
               ),
	      'status'  => array('#title' => 'Status', '#description'  => 'Wether published.',
              '#field' => 'status', '#value' => 0),
	      'sticky'  => array('#title' => 'Sticky', '#description'  => 'Wether the node is sticky.',
               '#field' => 'sticky', '#value' => 0),
	      'created' => array('#title' => 'Created', '#description' => 'When the node was created.',
               '#field' => 'created', '#value' => 0),
	      'changed' => array('#title' => 'Changed', '#description' => 'When the node was last changed.',
               '#field' => 'changed', '#value' => 0),
	      'nid'     => array('#title' => 'Unordered', '#description' => 'Sort by order placed in database (nid). In time, this sort becomes caotic.',
               '#field' => 'nid', '#value' => 1),
	      'type'    => array('#title' => 'Type', '#description' => 'The type of a node. The database holds this as a reference number.',
               '#field' => 'type', '#value' => 0),
	    ),
	  );
  protected static $admin_defaults = array();


  /**
   * Get the admin defaults. Theey are mildly cached in a static, but you can 
   * expire cache if you want.
   * @param <type> $expire
   * @return <type> 
   */
  static function admin($expire = TRUE) {
    if ($expire) {
      self::$admin_defaults = array();
    }
    self::adminConstruct();
    return self::$admin_defaults;
  }


  /**
   * Construct a full admin defaults array from the fixed options array and this
   * function's dynamic additions.
   */
  private static function adminConstruct() {
        if (!empty(self::$admin_defaults)) {
          return self::$admin_defaults;
        }

    self::$admin_defaults = self::$options_fixed;

    // Make all first level options into a checkbox,
    // And assert the '#value', and some '#child_value', fields
    foreach (self::$admin_defaults as &$group) {
      foreach ($group as $oid => &$opt) {
        if ($oid[0] == '#') continue;
        $opt['#type'] = 'ttm_checkbox_raw';
        $opt['#value'] = ($opt['#value'] != 0);
        //if (isset($opt['ttm_toggle'])) {
          //$opt['#child_value'] = $opt['ttm_toggle']['#value'];
        //}
      }
    }

    // Sorts. Glue a ttm_toggle in.
    // Overide the default #child_value and add the weight
    $i = 0;
    foreach (self::$admin_defaults['sort'] as $oid => &$opt) {
      if ($oid[0] == '#') continue;
      $opt['#children'] = array(
                    'ttm_toggle' => array(
                            '#type'        => 'ttm_toggle',
                            '#titles'       => array('ASC', 'DESC'),
                            '#value'       => 1,
                            ),
                        );
      //$opt['#child_value'] = 1;
      $opt['#weight'] = $i++;
    }

    // Filter

    // Add 'node types' dynamic info.
    $ttm_value = array();
    $result = db_query('SELECT type, name FROM {node_type}');
    while($node_t = db_fetch_array($result)) {
      self::$admin_defaults['filter']['node_types']['#children'][$node_t['type']] =  array(
                    '#type'        => 'ttm_checkbox_raw',
                    '#title'       => $node_t['name'],
                    '#value'       => 1,
                    );
    }

    // Fields
    // Add #weight
    $i = 0;
    foreach (self::$admin_defaults['fields'] as $oid => &$opt) {
      if ($oid[0] == '#') continue;
      $opt['#weight'] = $i++;
    }
  }


  private static function getStrippedValue($opt) {
    if (count($opt['#children']) == 1) {
      return $opt['#children']['ttm_toggle']['#value'];
    }
    $values = array();
    foreach($opt['#children'] as $cid => $child) {
      $values[$cid] = $child['#value'];
    }
    return $values;
  }

  
  /**
   * Remove descriptive data from an admin options array,
   * leaving 'active', 'field' and maybe 'value' (if there are children).
   * @param <type> $optsGroup
   * @return <type>
   */
  protected static function strip($group, $fields = TRUE) {
    $filtered = array();
    foreach ($group as $oid => $opt) {
      if ($oid[0] == '#') continue;
      $filtered[$oid] = array('active' => $opt['#value']);
      if ($fields) {
         $filtered[$oid]['field'] = $opt['#field'];
      }
      if (isset($opt['#children'])) {
         $filtered[$oid]['value'] = self::getStrippedValue($opt);
      }
    }
    return $filtered;
  }

  
  /**
   * Remove descriptive data from an options array,
   * leaving 'active', 'field' and 'value'.
   * @param <type> $optsGroup
   * @return <type>
   */
  /*
  protected static function stripWeighedOpts($optsGroup) {
    $filtered = array();
    foreach ($optsGroup as $opt => $data) {
      $filtered[$opt] = array('field'=> $data['field'], 'value' => $data['value'], 'active' => $data['active']);
    }
    return $filtered;
  }


  protected static function stripWeighedOpts2($optsGroup) {
    $filtered = array();
    foreach ($optsGroup as $oid => $opt) {
      if ($oid[0] == '#') continue;
      $filtered[$oid] = array('field'=> $opt['#field'], 'active' => $opt['#value'], 'value' => self::getStripValue($opt));
    }
    return $filtered;
  }
*/
  /**
   * Assert an active defaults (stripped defaults) array has been built.
   * This just returns if a cache has been built, but you can always expre the cache.
   * @return <type>
   */
  /*
  private static function strippedConstruct() {
        if (empty(self::$admin_defaults)) {
          self::adminConstruct();
        }
        
        // If there's one in the static, use it.
        if (!empty(self::$db_defaults)) {
          return;
        }

    foreach (self::$admin_defaults['sort'] as $SOpt => $data) {
      if(!$data['active']) {
        continue;
      }
      self::$db_defaults['sort'][$SOpt] = array('field'=> $data['field'], 'value' => $data['value']);
    }

    foreach (self::$admin_defaults['filter'] as $SOpt => $data) {
      if(!$data['active']) {
        continue;
      }
      self::$db_defaults['filter'][$SOpt] = array('field'=> $data['field'], 'value' => $data['value']);
    }
  }
*/


  static function display($t = 'admin') {
    dpm('Options:');
    dpm(self::$admin_defaults);

  }
 
}

/**
 * Most of this is to take a stock admin defaults (includes descriptive data),
 * and push in the users current prefs, creating full admin data.
 */
class TTMOptsAdmin extends TTMOptsDefaults
{
  // Carries a admin default, subsequently altered by injection.
  protected $opts_data = array();


  function __construct() {
      $this->opts_data = self::admin();
  }
  function displayStatic() {
       dpm('Admin Options (prefs):');
       dpm($this->opts_data);
  }


  function get($group = NULL){
    if($group) {
      return $this->opts_data[$group];
    }
    else {
      return $this->opts_data;
    }
  }


  /**
   * Inject user vals into full admin data.
   * This is a bit wriggly, as a user vals are significantly different in
   * structure to the Form API leaning admin data.
   * @param <type> $opt
   * @param <type> $usr_val
   */
  private function injectUsrVals(&$opt, $usr_val) {
    if (count($opt['#children']) > 1) {
      foreach ($opt['#children'] as $sub_oid => &$sub_opt) {
        //dpm($this->prefs[$group][$oid]['value'][$sub_oid]);
        $sub_opt['#value'] = $usr_val[$sub_oid];
      }
    }
    else {
        $opt['#children']['ttm_toggle']['#value'] = $usr_val;
    }
  }


  /**
   * Inject general usr opts into full admin data.
   * @param <type> $gid
   * @param <type> $usr_opts
   */
  private function injectOpts($gid, $usr_opts) {
      foreach ($this->opts_data[$gid] as $oid => &$opt) {
         if ($oid[0] == '#') continue;
         $opt['#value'] = $usr_opts[$gid][$oid]['active'];
         if(!isset($usr_opts[$gid][$oid]['value'])) continue;
         self::injectUsrVals($opt, $usr_opts[$gid][$oid]['value']);
       }
  }

  /**
   * Inject weighed usr opts into full admin data.
   * This needs to do a sort and set a weight field (user ops are stored in the
   * chosen order, so don't need the field, but admin does).
   * @param <type> $gid
   * @param <type> $usr_opts
   */
  private function injectWeighedOpts($gid, $usr_opts) {
    //Right, the prefs array is in the order the user left it. So we need to
    // rearrange the default array into the new order. Stuff PHP coders live for.
    // If you can think of a better way, do say.
    $this->opts_data[$gid] = array_merge($usr_opts[$gid], $this->opts_data[$gid]);
    // Now we're reindexed, lets put the user values in
    // At this point I can't think of a way of doing this, so..
    $i = 0;
    foreach ($this->opts_data[$gid] as $oid => &$opt) {
               if ($oid[0] == '#') continue;
        $opt['#value'] = $usr_opts[$gid][$oid]['active'];
        $opt['#weight'] = $i++;
         if(!isset($usr_opts[$gid][$oid]['value'])) continue;
         self::injectUsrVals($opt, $usr_opts[$gid][$oid]['value']);
    }
  }


  /**
   * Public function summoning the above.
   * @param <type> $usr_opts
   */
  public function injectPrefs($usr_opts) {
    $this->injectWeighedOpts('sort', $usr_opts);
    $this->injectOpts('filter', $usr_opts);
    $this->injectWeighedOpts('fields', $usr_opts);
  }
  
  /**
   * A stripped version of the full opts array.
   * Retains 'active' 'field' and 'value' only.
   */
  public function strippedAll() {
    $opts = array();
    $opts['sort'] = self::strip($this->opts_data['sort']);
    $opts['filter'] = self::strip($this->opts_data['filter']);
    $opts['fields'] = self::strip($this->opts_data['fields']);
    return $opts;
  }
  
  // Callback for a uasort.
  // (We should never have to handle weights with similar numbers,
  // they are asserted unique before this function is used on them.)
  private static function reweigh($a, $b) {
    return ($a['weight'] < $b['weight']) ? -1 : 1;
  }

  static function groupActive($group) {
    $active = FALSE;
    foreach($group as $opt) {
      $active = ($active || $opt['active']);
    }
    return $active;
  }

  // TODO: I know it looks silly, but I don't trust unset(). Do you trust unset()?
  //  Do you know how it works?
  /**
   * Retrieve data from a form out, taking only 'active', 'field', and 'value' ,
   * so disposing of 'weight' (and anything else).
   * Please be careful with this, it only works on pre-stripped data.
   *
   * @param <type> $group
   * @return <type>
   */
  static function stripWeights(&$group) {
    $filtered = array();
    foreach ($group as $oid => &$opt) {
        $filtered[$oid] = array('active' => $opt['active'], 'field' => $opt['field']);
          if (isset($opt['value'])) {
              $filtered[$oid]['value'] = $opt['value'];
          }
    }
    return $filtered;
  }

  /**
   * Take the 'options' submission from a treemenu edit form, and mutate the
   * data to the protocol used in the database.
   *
   * There's not much to this, as the data is stripped before it goes into the
   * form. All we have to do is resort the weighed options, then strip them of
   * the 'weight' field.  We do check 'sort' has something in it, or it defaults.
   *
   * @param <type> $formOpts
   */
  static function formProcess(&$formOpts) {
    // Weighed tables retain all keys, so the user always gets the form as
    // they left it. That means they retain the 'active' property.
    uasort($formOpts['sort'], 'TTMOptsAdmin::reweigh');
    uasort($formOpts['fields'], 'TTMOptsAdmin::reweigh');
    $formOpts['sort'] = self::stripWeights($formOpts['sort']);
    $formOpts['fields'] = self::stripWeights($formOpts['fields']);
   
    if (!self::groupActive($formOpts['sort'])) {
      $defaults = self::admin(FALSE);
      $formOpts['sort'] = self::strip($defaults['sort'], FALSE);
    }
  }
}

