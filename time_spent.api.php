<?php
/**
 * @file
 * Hooks provided by the Time Spent module.
 */

/**
 * Time spent on the site is being updated.
 *
 * @param object $account
 *   Drupal user account object.
 * @param int $time_spent
 *   Time, in seconds, a user has spent on the site.
 */
function hook_time_spent_site($account, $time_spent) {
  // React to this information as needed.
}

/**
 * Time spent on a given node is being updated.
 *
 * @param object $account
 *   Drupal user account object.
 * @param int $nid
 *   The Drupal node nid being tracked.
 * @param int $time_spent
 *   Time, in seconds, a user has spent on the site.
 */
function hook_time_spent_node($account, $nid, $time_spent) {
  // React to this information as needed.
}
