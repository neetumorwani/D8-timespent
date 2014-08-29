<?php

/**
 * @file
 * Administrative forms and reports for the Time Spent module.
 */

/**
 * The configuration form.
 */
function time_spent_admin_settings() {
  $form['who_counts'] = array(
    '#type' => 'fieldset',
    '#title' => t('Specify what and who this module will track'),
    '#description' => t('Set the node types and roles you want to have statistics. All them are tracked by default.'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE
  );
  $types = array();
  foreach (node_type_get_types() as $type) {
    $types[$type->type] = t($type->name);
  }
  $form['who_counts']['time_spent_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Node types'),
    '#default_value' => variable_get('time_spent_node_types', array()),
    '#options' => $types
  );
  $form['who_counts']['time_spent_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles'),
    '#default_value' => variable_get('time_spent_roles', array()),
    '#description' => t('If you want to track anonymous users, use Google Analytics.'),
    '#options' => user_roles(true)
  );
  $form['who_counts']['time_spent_timer'] = array(
    '#type' => 'textfield',
    '#title' => t('Seconds interval'),
    '#default_value' => variable_get('time_spent_timer', TIME_SPENT_TIMER),
  '#description' => t('We need to check by ajax if the user is on page yet. Define here the amount of time between one call and another.'),
  );
  $form['who_counts']['time_spent_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Define in minutes how long these ajax call should be tracked'),
    '#default_value' => variable_get('time_spent_limit', TIME_SPENT_LIMIT),
  '#description' => t('As we are using ajax call, session will never expire. So we need to avoid continuos tracking if the user left the chair with the page open.'),
  );
  return system_settings_form($form);
}

/**
 * Page callback to generate a report of time spent on a page.
 */
function time_spent_report() {
  $header = array(t('Post'), t('User'), t('Time Spent'));
  $rows = array();
  $output = "";
  $pager = 25;

  $query = db_select('time_spent_page', 'time_spent_page')
    ->fields('time_spent_page', array('timespent'))
    ->fields('users', array('uid'))
    ->fields('node', array('nid', 'title'));
  $query->join('node', 'node', 'node.nid = time_spent_page.nid');
  $query->join('users', 'users', 'users.uid = time_spent_page.uid');
  $result = $query->extend('PagerDefault')
    ->limit($pager)
    ->addTag('node_access');

  $nodes = $result->execute()->fetchAllAssoc('nid');

  foreach ($nodes as $row) {
  $user = user_load($row->uid);
  $rows[] = array(
        l($row->title, 'node/' . $row->nid),
        theme('username', array('account' => $user)),
        time_spent_sec2hms($row->timespent)
      );
  }
  $output .= "<br />";
  $output .= t("<h3>Time spent on each node page by users</h3>");
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('style' => 'width:600px')));
  $output .= theme('pager');


  $header = array(t('User'), t('Time Spent'));
  $rows = array();
  $query = db_select('time_spent_site', 'time_spent_site')
    ->fields('time_spent_site', array('timespent'))
    ->fields('users', array('uid', 'name'));
  $query->join('users', 'users', 'users.uid = time_spent_site.uid');
  $result = $query->extend('PagerDefault')
    ->limit($pager)
    ->addTag('node_access');

  $nodes = $result->execute()->fetchAllAssoc('name');
  foreach ($nodes as $row) {
  $user = user_load($row->uid);
  $rows[] = array(
        theme('username', array('account' => $user)),
        time_spent_sec2hms($row->timespent)
      );
  }
  $output .= "<br />";
  $output .= t("<h3>Time spent on the entire site by each user</h3>");
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('style' => 'width:600px')));
  $output .= theme('pager', array('element' => 1));

  return $output;
}

/**
 * Page callback for a report users time spent on the site.
 */
function time_spent_record() {
  $output = "";
  $a = drupal_get_form('time_spent_form');
  $output = render($a);
  $pager = 25;
  $header = array(t('User'), t('Time Spent'));
  $rows = array();
  $query = db_select('time_spent_site', 'time_spent_site')
    ->fields('time_spent_site', array('timespent'))
    ->fields('users', array('uid', 'name'));
  $query->join('users', 'users', 'users.uid = time_spent_site.uid');
  $result = $query->extend('PagerDefault')
    ->limit($pager)
    ->addTag('node_access');

  $nodes = $result->execute()->fetchAllAssoc('name');
  /*echo "<pre>";print_r (count($nodes));exit;*/
  $number = count($nodes);
  foreach ($nodes as $row) {
  $user = user_load($row->uid);
  $rows[] = array(
    l($user->name,'admin/reports/time_spent/timespent-list-users/'.$user->uid),
    time_spent_sec2hms($row->timespent)
      );
  }
  $output .= "<br />";
  $output .= "<h3>".t("Time spent on the entire site by each user")."</h3>";
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('style' => 'width:600px')));
  if($number > 25)
  $output .= theme('pager', array('element' => 1));
  return $output;
}

/**
 * A form for filtering by user in the various reports.
 */
function time_spent_form() {
  $form = array();

  $form['username'] = array(
    '#title' => t('username'),
    '#type' => 'textfield',
    '#autocomplete_path' => 'user/autocomplete',
   );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('See Report'),
    );
  $form['#submit'][] = 'time_spent_form_submit';
  return $form;
}

/**
 * Submit handler for the time spent report form.
 */
function time_spent_form_submit($form,&$form_state) {
  global $base_url;
  $user_name = $form_state['values']['username'];
  $query = db_select('users','u');
  $query->fields('u',array('uid'))
        ->condition('u.name',$user_name)
        ->range(0,1);
  $result = $query->execute()->fetchObject();
  drupal_goto($base_url.'/admin/reports/time_spent/timespent-list-users/'.$result->uid);
}

/**
 * Page callback for a detail page report by user.
 */
function time_spent_detail($account) {
  $userid=$account->uid;
  $header = array(t('Post'), t('User'), t('Time Spent'));
  $rows = array();
  $output = "";
  $pager = 25;

  $query = db_select('time_spent_page', 'time_spent_page')
    ->fields('time_spent_page', array('timespent'))
    ->fields('users', array('uid'))
    ->fields('node', array('nid', 'title'))
    ->condition('users.uid',$userid,'=');
  $query->join('node', 'node', 'node.nid = time_spent_page.nid');
  $query->join('users', 'users', 'users.uid = time_spent_page.uid');
  $result = $query->extend('PagerDefault')
    ->limit($pager)
    ->addTag('node_access');

  $nodes = $result->execute()->fetchAllAssoc('nid');
  $number = count($nodes);
  foreach ($nodes as $row) {
  $user = user_load($row->uid);
  $rows[] = array(
    l($row->title, 'node/' . $row->nid),
    theme('username', array('account' => $user)),
    time_spent_sec2hms($row->timespent)
    );
  }

  $output .= "<br />";
  $output .= "<h3>".t("Time spent on each node page by user")."</h3>";
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('style' => 'width:600px')));
  if($number > 25)
  $output .= theme('pager');
  return $output;
}
