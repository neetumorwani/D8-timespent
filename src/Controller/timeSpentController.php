<?php

/**
 * @file
 * Contains \Drupal\user\Controller\d8DemoController.
 */

namespace Drupal\time_spent\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\user\UserInterface;
use Drupal\node\NodeInterface;
use Drupal\Component\Utility\String;
use Drupal\Core\DependencyInjection\ContainerInjectionInterface;
use Drupal\Core\Database\Connection;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Controller routines for user routes.
 */
class timeSpentController extends ControllerBase {
  /**
   * Simple content Controller
   */
  public function renderContent() {
    $items = array('arg1' => 'Demo Content item 1', 'arg2' => 'Demo Content item 2');
    $variables = array(
      '#theme' => 'time_spent_content',
      '#items' => $items,
      '#empty' => $this->t('No sessions available')
    );
    return drupal_render($variables);
  }
}
