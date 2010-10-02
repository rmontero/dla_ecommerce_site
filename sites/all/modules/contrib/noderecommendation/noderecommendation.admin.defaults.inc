<?php
// $Id: noderecommendation.admin.defaults.inc,v 1.1.2.2 2009/06/01 16:23:38 hanoii Exp $

function noderecommendation_admin_defaults_nodes() {
  $limit = 25;
  $element = 0;
  $result = pager_query("SELECT * FROM {noderecommendation_node}", $limit, $element);

  $h = array();
  $h[] = t('Node');
  $h[] = t('Default');
  $h[] = t('Operations');

  while ($row = db_fetch_object($result)) {
    $node = node_load($row->nid);
    $r = array();
    $r[] = l($node->title, 'node/' . $node->nid);
    $info = _noderecommendation_to_arrayinfo(unserialize($row->data));
    $r[] = array(
      'data' => theme('noderecommendation_defaults', $info),
      'width' => '60%',
    );
    $r[] = l(t('edit'), 'node/' . $row->nid . '/noderecommendation', array('query' => 'destination=admin/content/noderecommendation/nodes')) . ' / ' . l(t('delete'), 'node/' . $row->nid . '/noderecommendation/delete', array('query' => 'destination=admin/content/noderecommendation/nodes'));
    $rows[] = $r;
  }

  if (count($rows)) {
    $output .= theme('table', $h, $rows);
  } else {
    $output .= t('<p><strong>There are no nodes with custom defaults.</strong></p>');
  }
  $output .= theme('pager', array(), $limit, $element);

  return $output;
}


function noderecommendation_admin_defaults() {
  $result = db_query("SELECT * FROM {noderecommendation_defaults}");

  $h = array();
  $h[] = t('Id');
  $h[] = t('Terms');
  $h[] = t('Default');
  $h[] = t('Operations');
  while ($row = db_fetch_object($result)) {
    $data = array();
    $info = array();
    $result_terms = db_query("SELECT * FROM {noderecommendation_defaults_terms} WHERE nrdid = %d", $row->nrdid);
    while ($row_term = db_fetch_object($result_terms)) {
      $term = taxonomy_get_term($row_term->tid);
      $data[$term->vid][] = $term->name;
    }
    foreach ($data as $vid => $term_names) {
      $vocabulary = taxonomy_vocabulary_load($vid);
      $info[] = "<b>{$vocabulary->name}:</b> " . implode(', ', $term_names);
    }
    $r = array();
    $r[] = $row->nrdid;
    $r[] = implode('<br/>', $info);
    $info = _noderecommendation_to_arrayinfo(unserialize($row->data));
    $r[] = array(
      'data' => theme('noderecommendation_defaults', $info),
      'width' => '60%',
    );
    $r[] = l(t('edit'), 'admin/content/noderecommendation/' . $row->nrdid . '/edit') . ' / ' . l(t('delete'), 'admin/content/noderecommendation/' . $row->nrdid . '/delete');
    $rows[] = $r;
  }

  return theme('table', $h, $rows);
}

function noderecommendation_admin_defaults_delete_submit($form, &$form_state) {
  $nrdid = $form_state['values']['nrdid'];
  db_query("DELETE FROM {noderecommendation_defaults_terms} WHERE nrdid = %d", $nrdid);
  db_query("DELETE FROM {noderecommendation_defaults} WHERE nrdid = %d", $nrdid);

  drupal_set_message(t('Node recommendation default deleted.'));
  $form_state['redirect'] = 'admin/content/noderecommendation';
}


function noderecommendation_admin_defaults_delete(&$form_state, $nrd) {
  $form['nrdid'] = array('#type' => 'value', '#value' => $nrd->nrdid);
  return confirm_form($form, t('Are you sure you want to delete this node recommendation default (Id: %id)?', array('%id' => $nrd->nrdid)), 'admin/content/noderecommendation', t('This action cannot be undone.'), t('Delete'), t('Cancel'), 'noderecommendation_admin_defaults_delete');
}

function noderecommendation_admin_defaults_edit(&$form_state, $nrd = null) {
  if ($nrd) {
    $default_values['default']['default'] = $nrd->data;
    // get taxonomy terms
    $result_terms = db_query("SELECT ndt.tid, td.vid FROM {noderecommendation_defaults_terms} ndt LEFT JOIN {term_data} td ON td.tid = ndt.tid WHERE ndt.nrdid = %d", $nrd->nrdid);
    while ($row = db_fetch_object($result_terms)) {
      $default_values['vocabularies'][$row->vid]['tids'][] = $row->tid;
    }
  }

  $form = array('#tree' => TRUE);

  $form['vocabularies'] = array(
    '#type' => 'fieldset',
    '#title' => t('Vocabularies'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Choose a combination of terms that this default should be applied.'),
  );
  $exclude_vids = variable_get('noderecommendation_exclude_vids', array());
  $vocabularies = taxonomy_get_vocabularies();
  foreach ($exclude_vids as $vid) {
    if ($vid) {
      unset($vocabularies[$vid]);
    }
  }
  foreach ($vocabularies as $vocabulary) {
      $terms = taxonomy_get_tree($vocabulary->vid);
      if (!empty($terms)) {
        $form['vocabularies'][$vocabulary->vid] = array(
          '#type' => 'fieldset',
          '#title' => $vocabulary->name,
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
        );
        $form['vocabularies'][$vocabulary->vid]['tids'] =  _taxonomy_term_select(t('Terms'), 'taxonomy', $default_values['vocabularies'][$vocabulary->vid]['tids'], $vocabulary->vid, '', 1, FALSE);
      }
  }
  $form['default'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Define the default settings for nodes with terms on the combination set above.'),
  );
  $form['default']['default'] = noderecommendation_admin_form($default_values['default']['default']);

  if ($nrd->nrdid) {
    $form['nrdid'] = array('#type' => 'value', '#value' => $nrd->nrdid);
    $form['buttons']['submit'] = array('#type' => 'submit', '#value' => t('Save'));
    $form['buttons']['delete'] = array('#type' => 'markup', '#value' => l(t('Delete'), 'admin/content/noderecommendation/' . $nrd->nrdid . '/delete'));
  }
  else {
    $form['buttons']['submit'] = array('#type' => 'submit', '#value' => t('Create'));
  }

  return $form;
}

function noderecommendation_admin_defaults_edit_validate($form, &$form_state) {
  // Try to found a default in the defaults database that matches something similar
  $nrdid = $form_state['values']['nrdid'];

  $args = array();
  $where_and = $where_or = array();
  $vids = 0;
  foreach ($form_state['values']['vocabularies'] as $vocabulary) {
    if (!empty($vocabulary['tids'])) {
      $vids++;
    }
    foreach ($vocabulary['tids'] as $tid) {
      $where_or[] = 'td.tid = %d';
      $args[] = $tid;
    }
  }

  if (!empty($where_or)) {
    $where_and[] = '(' . implode(' OR ', $where_or). ')';
  }

  if ($nrdid) {
    $where_and[] = 'nrdt.nrdid != %d';
    $args[] = $nrdid;
  }

  if (!empty($where_and)) {
    $where = 'WHERE ' . implode(' AND ', $where_and);
  }

  if (!$vids) {
    form_set_error('vocabularies', t('You need to select at least one term on one vocabulary.'));
  }
  else {
    $having = "HAVING vids = %d";
    $args[] = $vids;

    $query = "SELECT nrdt.nrdid, nrd.data, COUNT(DISTINCT td.tid) as tids, COUNT(DISTINCT td.vid) as vids FROM {noderecommendation_defaults_terms} nrdt LEFT JOIN {noderecommendation_defaults_terms} nrdt2 ON nrdt.nrdid = nrdt2.nrdid LEFT JOIN {noderecommendation_defaults} nrd ON nrd.nrdid = nrdt.nrdid LEFT JOIN {term_data} td ON td.tid = nrdt2.tid $where GROUP BY nrdt2.nrdid $having ORDER BY vids DESC, tids DESC";
    $default = db_fetch_object(db_query($query, $args));

    if ($default) {
      form_set_error('vocabularies', t('The combination of terms will conflict with another configured default (Id: %id). Please make sure that either of the defaults do not have matching terms across all vocabularies.', array('%id' => $default->nrdid)));
    }
  }
}

function noderecommendation_admin_defaults_edit_submit($form, &$form_state) {
  $nrdid = $form_state['values']['nrdid'];

  if (!$nrdid) {
    db_query("INSERT INTO {noderecommendation_defaults} (data) VALUES ('%s')", serialize($form_state['values']['default']['default']));
    $nrdid = db_last_insert_id('noderecommendation_defaults', 'nrdid');
    foreach ($form_state['values']['vocabularies'] as $vocabulary) {
      foreach ($vocabulary['tids'] as $tid) {
        if ($tid) {
          db_query("INSERT INTO {noderecommendation_defaults_terms} (tid, nrdid) VALUES (%d, %d)", $tid, $nrdid);
        }
      }
    }

    drupal_set_message(t('Node recommendation default has been created.'));
  }
  else {
    db_query("UPDATE {noderecommendation_defaults} SET data = '%s' WHERE nrdid = %d", serialize($form_state['values']['default']['default']), $nrdid);
    db_query("DELETE FROM {noderecommendation_defaults_terms} WHERE nrdid = %d", $nrdid);
    foreach ($form_state['values']['vocabularies'] as $vocabulary) {
      foreach ($vocabulary['tids'] as $tid) {
        if ($tid) {
          db_query("INSERT INTO {noderecommendation_defaults_terms} (tid, nrdid) VALUES (%d, %d)", $tid, $nrdid);
        }
      }
    }

    drupal_set_message(t('Node recommendation default has been updated.'));
  }

  $form_state['redirect'] = 'admin/content/noderecommendation';
}