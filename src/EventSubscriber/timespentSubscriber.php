<?php

namespace Drupal\time_spent\EventSubscriber;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Symfony\Component\HttpKernel\KernelEvents;
use Symfony\Component\HttpKernel\Event\GetResponseEvent;
use Symfony\Component\EventDispatcher\EventSubscriberInterface;
use Drupal\Component\Utility\Unicode;

class timespentSubscriber implements EventSubscriberInterface {
    public function timespentuserhasrole(GetResponseEvent $event) {
        if (time_spent_user_has_role()) {
        $timer = \Drupal::config(time_spent.settings)->get('time_spent_timer');
        $limit = \Drupal::config('time_spent.settings')->get('time_spent_limit');
        $mynide = -1; //false value
        //checks if this is a node page. taxonomy or views pages won't be counted.
        //confirm if this node type and user role will be tracked
        $nodetypes = \Drupal::config('time_spent.settings')->get('time_spent_node_types');
        if ($node = \Drupal::routeMatch()->getParameter()) {
          if (($nodetypes == 'all' || $nodetypes[$node->type] === $node->type )) {
            $mynide = $node->nid;
          }
        }
        $timespent = array(
            '#attached' => array(
                'js' => array(
                    drupal_get_path('module', 'time_spent') . '/js/time_spent.js' => array(),
                ),
            ),
        );
        drupal_add_js(drupal_get_path('module', 'time_spent') . '/time_spent.js');
        drupal_add_js(array('time_spent' => array('timer' => check_plain($timer), 'limit' => check_plain($limit), 'nid' => $mynide, 'sectoken' => drupal_get_token())), 'setting');
      }
    }

    /**
    * {@inheritdoc}
    */
    static function getSubscribedEvents() {
        $events[KernelEvents::REQUEST][] = array('timespentuserhasrole');
        return $events;
    }
}