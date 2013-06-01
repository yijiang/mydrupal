<?php

/**
 * @file
 * Page display functions for for Gigya module.
 */

/**
 * Page callback for the user/%user/edit/gigya page.
 *
 * @see gigya_menu()
 *
 * @return
 *   Gigya connect page HTML.
 */
function gigya_connect() {
  drupal_set_title(t('Edit your Social Network connections'));
  global $user;
  if ($user->uid === arg(1)) {
    return theme('gigya_connect_display');
  }
  return t('only users can edit thire own connections');
}

/**
 * socialize-login page callback.
 *
 * @see gigya_menu()
 */
function gigya_login_ajax() {
  require_once(drupal_get_path('module', 'gigya') . '/GSSDK.php');
  ctools_include('ajax');
  ctools_include('modal');
  ctools_add_js('ajax-responder');
  $parms = $_POST;
  //If the admin set a post login redirect send the user to it if no just reload the page.
  $dest =  variable_get('gigya_login_uiconfig_urlredirect', '');
  if (!empty($dest)) {
    $sucess_command = ctools_ajax_command_redirect($dest);
  }
  else {
    $sucess_command = ctools_ajax_command_reload();
  }
  //Check if the user is not logged in.
  if (!user_is_logged_in()) {
    $commands = array();
    $_SESSION['gigya_suppress_notifylogin_js'] = TRUE;
    module_load_include('inc', 'gigya');
    $secretkey = variable_get('gigya_SecretKey', '');
    if (SigUtils::validateUserSignature($parms['UID'], $parms['timestamp'], $secretkey, $parms['signature'])) {
      $gigya_user = $parms['user'];

      // Check to see if the Gigya user is a Drupal user.
      if (is_numeric($gigya_user['UID']) && $parms['isSiteUID'] === 'true' && $user = user_load($gigya_user['UID'])) {
        if (!variable_get('user_email_verification', TRUE)) {
          // We can suppress the notify login JS because the user is clearly
          // already logged into gigya.
          //$_SESSION['gigya_suppress_notifylogin_js'] = TRUE;
          // Log the user in.
          global $user;
          $user = user_load($parms['UID']);
          user_login_finalize();
          $commands[] = $sucess_command;
        }
        else {
          $commands[] = ajax_command_alert(t('You must validate your email address for this account before logging in.'));
        }
        return array('#type' => 'ajax', '#commands' => $commands);
      }
      // If the user isn't a Drupal user, try to register them if allowed.
      elseif (variable_get('user_register', 1)) {
        $form_state['values'] = array();
        $uname = $parms['user']['email'];
        $user_existes = (bool) db_select('users')->fields('users', array('uid'))->condition('mail', db_like($uname), 'LIKE')->range(0, 1)->execute()->fetchField() || (bool) db_select('users')->fields('users', array('uid'))->condition('name', db_like($uname), 'LIKE')->range(0, 1)->execute()->fetchField();
        if ($user_existes) {
          $form_state = array(
            'title' => t('Please login to link the accounts'),
            'ajax' => TRUE,
            'email' => $parms['user']['email'],
          );
          $commands = ctools_modal_form_wrapper('gigya_link_accounts_form', $form_state);
          return array('#type' => 'ajax', '#commands' => $commands);
        }
        else {
          $form_state['values']['name'] = $uname;
        }
        $form_state['values']['mail'] = $uname;
        $form_state['values']['pass'] = user_password();
        $_SESSION['gigya_uid'] = $parms['user']['UID'];

        // Allow other modules to change this info before we create the user.
        drupal_alter('gigya_create_user', $form_state, $parms);
        drupal_form_submit('user_register_form', $form_state);

        if (form_get_errors()) {
          if (variable_get('gigya_debug', 0)) {
            $messages = drupal_get_messages('error');
            $output = print_r($messages, TRUE);
            // We were unable to register a valid new user.
            // Send the error messages to watchdog.
            watchdog('gigya', 'Unable to auto-create user account:' . $output);
          }
          //if there is an error in the registration process, return the registration form in a modal.
          $form_state = array(
            'title' => t('Please fill-in missing details'),
            'ajax' => TRUE,
            'gigya' => TRUE,
          );
          $commands = ctools_modal_form_wrapper('user_register_form', $form_state);
          return array('#type' => 'ajax', '#commands' => $commands);
        }
        // The register process is done.
        $commands[] = $sucess_command;
        return array('#type' => 'ajax', '#commands' => $commands);
      }
      else {
        $commands[] = ajax_command_alert(t('Only site administrators can create new user accounts.'));
        return array('#type' => 'ajax', '#commands' => $commands);
      }
    }
    $commands[] = ajax_command_alert(t('Gigya signature not valid.'));
    return array('#type' => 'ajax', '#commands' => $commands);
  }
  $commands[] = $sucess_command;
  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Get the user friends list from Gigya.
 *
 * @param $account
 *   The user object.
 *
 * @return
 *   HTML for friends list.
 */
function gigya_get_friends($account) {
  $title = isset($account->title) ? $account->title : $account->name;
  drupal_set_title(check_plain($title));

  module_load_include('inc', 'gigya');
  $site_friends = array();
  $this_gigya_user = new GigyaUser($account->uid);
  if ($friends = $this_gigya_user->getFriends(array('siteUsersOnly' => TRUE))) {
    if (!empty($friends['friends'])) {
      foreach ($friends['friends'] as $friend) {
        if ($friend['isSiteUser'])
          $site_friends[] = $friend;
      }
    }
  }
  return theme('gigya_friends', array('friends' => $site_friends));
}

/**
 * Wrapper for the Gigya Javascript userInfo function.
 *
 * Injects the Gigya getUserInfo Javascript function.
 *
 * @param $account
 *   The user object.
 *
 * @return
 *   The user info from Gigya.
 */
function gigya_get_user($account) {
  $title = isset($account->title) ? $account->title : $account->name;
  drupal_set_title(check_plain($title));

  // Add the onload functions .
  $behavior_js = 'Drupal.behaviors.gigyaGetUserInfo = { attach: function(context, settings) { gigya.services.socialize.getUserInfo(Drupal.settings.gigya.conf, {callback:Drupal.gigya.getUserInfoCallback}); }}';
  drupal_add_js($behavior_js, array('type' => 'inline', 'scope' => JS_DEFAULT));

  $userinfo = '<div id="userinfo"></div>';
  return $userinfo;
}


function gigya_register_ajax($form, $form_state) {
  ctools_include('ajax');
  ctools_include('modal');
  ctools_add_js('ajax-responder');
  drupal_process_form('user_register_form', $form, $form_state);
  $commands[] = ctools_modal_command_dismiss();
  $dest =  variable_get('gigya_login_uiconfig_urlredirect', '');
  if (!empty($dest)) {
    $commands[] = ctools_ajax_command_redirect($dest);
  }
  else {
    $commands[] = ctools_ajax_command_reload();
  }
  return array('#type' => 'ajax', '#commands' => $commands);
}
