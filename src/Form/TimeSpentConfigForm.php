<?php

/**
 * @file
 * Contains \Drupal\time_spent\Form\TimeSpentConfigForm.
 */

namespace Drupal\time_spent\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\node\Entity\NodeType;

class TimeSpentConfigForm extends ConfigFormBase {
  public function getFormId() {
    return 'time_spent_config_form';
  }
  public function buildForm(array $form, array &$form_state) {
    $form['who_counts'] = array(
      '#type' => 'fieldset',
      '#title' => $this->t('Specify what and who this module will track'),
      '#description' => $this->t('Set the node types and roles you want to have statistics. All them are tracked by default.'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE
    );

    // Form an array of node types to be used in the config form
    $types = array();
    foreach (NodeType::loadMultiple() as $type) {
      $types[$type->type] = t($type->name);
    }

    // Form an array of user roles to be used in the config form
    $user_roles = array();
    foreach(user_roles(TRUE) as $key => $role) {
      $user_roles[$key] = $key;
    }

    $form['who_counts']['time_spent_node_types'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Node types'),
      '#default_value' => \Drupal::config('time_spent.settings')->get('time_spent_node_types'),
      '#options' => $types
    );
    $form['who_counts']['time_spent_roles'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Roles'),
      '#default_value' => $this->config('time_spent.settings')->get('time_spent_roles'),
      '#description' => t('If you want to track anonymous users, use Google Analytics.'),
      '#options' => $user_roles
    );
    $form['who_counts']['time_spent_timer'] = array(
      '#type' => 'textfield',
      '#title' => t('Seconds interval'),
      '#default_value' => $this->config('time_spent.settings')->get('time_spent_timer'),
      '#description' => t('We need to check by ajax if the user is on page yet. Define here the amount of time between one call and another.'),
    );
    $form['who_counts']['time_spent_limit'] = array(
      '#type' => 'textfield',
      '#title' => t('Define in minutes how long these ajax call should be tracked'),
      '#default_value' => $this->config('time_spent.settings')->get('time_spent_limit'),
      '#description' => t('As we are using ajax call, session will never expire. So we need to avoid continuos tracking if the user left the chair with the page open.'),
    );

    return parent::buildForm($form, $form_state);
  }

  public function submitForm(array &$form, array &$form_state) {
    $values = $form_state['values'];
    $config = $this->configFactory->get('time_spent.settings');
    $config->set('time_spent_node_types', $values['time_spent_node_types']);
    $config->set('time_spent_roles', $values['time_spent_roles']);
    $config->set('time_spent_timer', $values['time_spent_timer']);
    $config->set('time_spent_limit', $values['time_spent_limit']);
    $config->save();
    parent::submitForm($form, $form_state);
  }
}
