<?php

/**
 * @file
 * Provides the administration settings for the Gigya module.
 */

/**
 * Form builder for the Gigya administration form.
 *
 * @see gigya_admin_settings_form_submit()
 * @return array
 *   The Gigya administration form.
 */
function gigya_admin_form($form, &$form_state) {
  global $user, $base_path;
  include_once DRUPAL_ROOT . '/' . 'modules/user/user.pages.inc';
  module_load_include('inc', 'gigya');
  $form = array();

  $form['keys'] = array(
    '#type' => 'fieldset',
    '#title' => t('Gigya Socialize keys'),
    '#description' => t('Keys needed for connecting to the Gigya Socialize service.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => -400,
  );
  $form['keys']['gigya_APIKey'] = array(
    '#type' => 'textfield',
    '#title' => t('Gigya API Key'),
    '#description' => t('Specify the Gigya API key for this domain'),
    '#default_value' => variable_get('gigya_APIKey', ''),
    '#required' => TRUE,
    '#weight' => -300,
  );
  $form['keys']['gigya_SecretKey'] = array(
    '#type' => 'textfield',
    '#title' => t('Gigya Secret Key'),
    '#description' => t('Specify the Gigya Secret (Base64 encoded) key for this domain'),
    '#default_value' => variable_get('gigya_SecretKey', ''),
    '#required' => TRUE,
    '#weight' => -200,
  );
  $from['gigya_networks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Giya network providers'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['gigya_networks']['gigya_network_list'] = array(
    '#type' => 'textfield',
    '#title' => t('Providers'),
    '#size' => 60,
    '#maxlength' => 400,
    '#description' => t('Comma separated list of networks that would be included. For example: "Facebook, Twitter, Yahoo". <br />* means all networks. See list of available !providers', array('!providers' => l(t('providers.'), 'http://developers.gigya.com/020_Client_API/020_Methods/Socialize.showLoginUI', array('external' => TRUE, 'attributes' => array('target' => '_blank'))))),
    '#default_value' => variable_get('gigya_network_list', '*'),
  );
  $form['gigya_language'] = array(
    '#type' => 'select',
    '#title' => t('Interface language'),
    '#description' => t('Please select the interface language'),
    '#options' => gigya_get_languages(),
    '#default_value' => variable_get('gigya_language', 'en'),
  );
  $form['gigya_social_analytics'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Google Social Analytics'),
    '#default_value' => variable_get('gigya_social_analytics', FALSE),
  );
  $form['gigya_advanced_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Gigya advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $extra_parms = variable_get('gigya_global_extra', array());
  $form['gigya_advanced_settings']['gigya_global_extra'] = array(
    '#type' => 'textarea',
    '#title' => t('Custom Global Conf'),
    '#rows' => 5,
    '#element_validate' => array('gigya_key_val_filed_validate'),
    '#description' => t('Enter values in key|value format. See list of available !parameters', array('!parameters' => l(t('parameters'), 'http://developers.gigya.com/030_API_reference/010_Client_API/010_Objects/Conf_object', array('external' => TRUE, 'attributes' => array('target' => '_blank'))))),
    '#default_value' => !empty($extra_parms) ? gigya_values_array_to_string($extra_parms) : '',
    '#attributes' => array('placeholder' => 'facebookExtraPermissions|rsvp_event,sms'),
  );
  $form['gigya_advanced_settings']['gigya_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Gigya debuging'),
    '#description' => t('Write errors to watchdog'),
    '#default_value' => variable_get('gigya_debug', FALSE),
  );
  return system_settings_form($form);
}

/*
 * LoginUI
 */
function gigya_login_admin_form($form, $form_state) {
  // Set up colors. If colorpicker module is enabled, we'll use that instead.
  $color_field_type = (function_exists('colorpicker_2_or_later') ? 'colorpicker_' : '') . 'textfield';
  $form['gigya_login_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Gigya Social login'),
    '#default_value' => variable_get('gigya_login_enabled', TRUE),
  );
  $form['gigya_login_surpress_login'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Gigya login only'),
    '#default_value' => variable_get('gigya_login_surpress_login', FALSE),
    '#description' => t('Checking this option will disable normal Drupal login. Admins can login via /user'),
  );
  $form['gigya_login_buttons_style'] = array(
    '#type' => 'select',
    '#title' => t('Button style'),
    '#options' => array(
      'standard' => t('Icons'),
      'fullLogo' => t('Full logos'),
    ),
    '#default_value' => variable_get('gigya_login_buttons_style', 'standard'),
  );
  $form['gigya_login_uiconfig_urlredirect'] = array(
    '#type' => 'textfield',
    '#title' => t('URL redirect after login'),
    '#default_value' => variable_get('gigya_login_uiconfig_urlredirect', ''),
    '#description' => t('Choose an alternate URL to direct users to after login. Defaults to the current page. Do not use a beginning or ending slash!'),
  );
  $form['gigya_login_uiconfig_terms'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show terms link'),
    '#description' => t('Display the Gigya terms of service link'),
    '#default_value' => variable_get('gigya_login_uiconfig_terms', FALSE),
  );
  $form['gigya_login_plugin_size'] = array(
    '#type' => 'checkbox',
    '#title' => t('Set custom size for plugin'),
    '#default_value' => variable_get('gigya_login_plugin_size', FALSE),
  );
  $form['gigya_login_plugin_width'] = array(
    '#prefix' => '<div class="height-with">',
    '#type' => 'textfield',
    '#title' => t('W:'),
    '#size' => 5,
    '#states' => array(
      'visible' => array(
        '#edit-gigya-login-plugin-size' => array('checked' => TRUE),
      ),
    ),
    '#default_value' => variable_get('gigya_login_plugin_width', GIGYA_DEFAULT_LOGINUI_WIDTH),
  );
  $form['gigya_login_plugin_height'] = array(
    '#type' => 'textfield',
    '#title' => t('H:'),
    '#size' => 5,
    '#states' => array(
      'visible' => array(
        '#edit-gigya-login-plugin-size' => array('checked' => TRUE),
      ),
    ),
    '#default_value' => variable_get('gigya_login_plugin_height', GIGYA_DEFAULT_LOGINUI_HEIGHT),
    '#suffix' => '</div>',
  );
  $form['gigya_login_plugins_login'] = array(
    '#type' => 'select',
    '#title' => t('Login behavior'),
    '#options' => array(
      'loginExistingUser' => t('Login existing user'),
      'alwaysLogin' => t('Always login')
    ),
    '#default_value' => variable_get('gigya_login_plugins_login', 'loginExistingUser'),
  );
  $form['gigya_login_advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced showLoginUI settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 1,
  );
  $form['gigya_login_advanced']['gigya_login_uiconfig_containerID'] = array(
    '#type' => 'textfield',
    '#title' => t('showLoginUI container ID'),
    '#default_value' => variable_get('gigya_login_uiconfig_containerID', GIGYA_DEFAULT_LOGINUI_CONTAINERID),
    '#description' => t('div ID for loginUI'),
    '#required' => TRUE,
  );
  $login_extra_parms = variable_get('gigya_login_extra', array());
  $form['gigya_login_advanced']['gigya_login_extra'] = array(
    '#type' => 'textarea',
    '#title' => t('Advanced showLoginUI settings'),
    '#rows' => 5,
    '#element_validate' => array('gigya_key_val_filed_validate'),
    '#description' => t('Enter values in key|value format. See list of available !parameters', array('!parameters' => l(t('parameters'), 'http://developers.gigya.com/030_Gigya_Socialize_API_2.0/030_API_reference/010_Client_API_%28JavaScript%29/Social_service/Socialize.showLoginUI', array('external' => TRUE, 'attributes' => array('target' => '_blank')) ))),
    '#default_value' => !empty($login_extra_parms) ? gigya_values_array_to_string($login_extra_parms) : '',
  );

  // ConnectUI
  $form['gigya_connect_advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced showAddConnectionsUI settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 2,
  );
  $form['gigya_connect_advanced']['gigya_connect_uiconfig_containerID'] = array(
    '#type' => 'textfield',
    '#title' => t('showAddConnectionsUI container ID:'),
    '#default_value' => variable_get('gigya_connect_uiconfig_containerID', GIGYA_DEFAULT_CONNECTUI_CONTAINERID),
    '#description' => t('div ID for showAddConnectionsUI'),
  );
  $connect_extra_parms = variable_get('gigya_connect_extra', array());
  $form['gigya_connect_advanced']['gigya_connect_extra'] = array(
    '#type' => 'textarea',
    '#title' => t('Advanced showAddConnectionsUI settings'),
    '#rows' => 5,
    '#element_validate' => array('gigya_key_val_filed_validate'),
    '#description' => t('Enter values in key|value format. See list of available !parameters', array('!parameters' => l(t('parameters'), 'http://developers.gigya.com/020_Client_API/020_Methods/socialize.showAddConnectionsUI', array('external' => TRUE, 'attributes' => array('target' => '_blank')) ))),
    '#default_value' => !empty($connect_extra_parms) ? gigya_values_array_to_string($connect_extra_parms) : '',
  );
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'gigya') . '/css/gigya_admin.css',
  );

  return system_settings_form($form);
}

/**
 * Implements hook_form_validate().
 */
function gigya_admin_form_validate($form, &$form_state) {
  // Vaidate the the api key and secret key
  if (gigya_validate($form_state['values']['gigya_APIKey'], $form_state['values']['gigya_SecretKey']) == FALSE) {
    form_set_error('', t('Your API key or Secret key are not valid'));
  }
}

/**
 * Form builder for the Gigya sharebar administration form.
 *
 * @return array
 *   The Gigya sharebar administration form.
 */
function gigya_sharebar_settings_form() {
  $form['gigya_share_message'] = array(
    '#markup' => t('To position a Gigya Share Bar plugin, place a "Gigya Share Bar" field via the "Structure"/"Content types" menu'),
    '#weight' => -50,
  );
  $form['gigya_share_global'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sharing global configuration'),
    '#collapsible' => FALSE,
    '#weight' => -10,
  );
  $form['gigya_share_global']['gigya_share_container_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Container ID'),
    '#size' => 60,
    '#default_value' => variable_get('gigya_share_container_id', 'gigya-share') ,
  );
  $form['gigya_share_global']['gigya_share_buttons'] = array(
    '#type' => 'textfield',
    '#title' => t('Providers'),
    '#size' => 60,
    '#maxlength' => 400,
    '#default_value' => variable_get('gigya_share_buttons', GIGYA_DEFAULT_SHARE_BOTTONS),
    '#description' => t('List of relevant values is available !here under the "shareButtons" parameter', array('!here' => l(t('here'), 'http://developers.gigya.com/020_Client_API/020_Methods/socialize.showShareBarUI', array('external' => TRUE, 'attributes' => array('target' => '_blank'))))),
  );
  $form['gigya_share_global']['gigya_share_container_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Container ID'),
    '#size' => 60,
    '#default_value' => variable_get('gigya_share_container_id', 'gigya-share'),
  );
  $form['gigya_share_global']['gigya_share_layout'] = array(
    '#type' => 'select',
    '#title' => t('Share Bar layout'),
    '#options' => array(
      'horizontal' => t('Horizontal'),
      'vertical' => t('Vertical'),
    ),
    '#default_value' => variable_get('gigya_share_layout', 'horizontal'),
  );
  $form['gigya_share_global']['gigya_share_showcounts'] = array(
    '#type' => 'select',
    '#title' => t('Show counts'),
    '#description' => t('Where to display the share counts in relation to the share button'),
    '#options' => array(
      'right' => t('Right'),
      'top' => t('Top'),
      'none' => t('none'),
    ),
    '#default_value' => variable_get('gigya_share_showcounts', 'right'),
  );
  $form['gigya_share_global']['gigya_share_af_scope'] = array(
    '#type' => 'select',
    '#title' => t('Scope for publishing to Activity Feed'),
    '#description' => t('When publishing feed items, by default the feed items are published to social networks only and will not appear on the site\'s Activity Feed plugin. To change this behavior, you must change the publish scope to "Both".'),
    '#options' => array(
      'external' => t('External'),
      'both' => t('Both'),
    ),
    '#default_value' => variable_get('gigya_share_af_scope', 'external'),
  );
  $form['gigya_share_global']['gigya_share_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Default Share title'),
    '#size' => 60,
    '#default_value' => variable_get('gigya_share_title', '[node:title]'),
  );
  $form['gigya_share_global']['gigya_share_desc'] = array(
    '#type' => 'textfield',
    '#title' => t('Default share description'),
    '#size' => 60,
    '#default_value' => variable_get('gigya_share_desc', '[node:summary]'),
  );
  $form['gigya_share_global']['gigya_share_link_back'] = array(
    '#type' => 'textfield',
    '#title' => t('Default linkback URL'),
    '#size' => 60,
    '#default_value' => variable_get('gigya_share_link_back', '[node:url:absolute]'),
  );
  $form['gigya_share_global']['gigya_share_image_behavior'] = array(
    '#type' => 'radios',
    '#title' => t('Image'),
    '#options' => array(
      'default' => t('Use image tag if exists, first image on post otherwise.'),
      'first' => t('First image on the post'),
      'url' => t('Specify an image URL'),
    ),
    '#default_value' => variable_get('gigya_share_image_behavior', 'default'),
    '#attributes' => array('class' => array('image-behv')),
  );
  $form['gigya_share_global']['gigya_share_image_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Default URL of the image to share'),
    '#size' => 20,
    '#states' => array(
      'visible' => array(
        '.image-behv' => array('value' => 'url'),
      ),
    ),
    '#default_value' => variable_get('gigya_share_image_url', ''),
  );
  $form['gigya_share_global']['gigya_share_short_url'] = array(
    '#type' => 'checkbox',
    '#title' => t('Share using short URLs'),
    '#default_value' => variable_get('gigya_share_short_url', FALSE),
  );
  $form['gigya_share_global']['gigya_share_tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Token list'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['gigya_share_global']['gigya_share_tokens']['tokens_list'] = array(
    '#markup' => theme('token_tree', array('token_types' => array('node'), 'global_types' => FALSE)),
  );
  $form['gigya_share_global']['gigya_advanced_sharing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced Share Bar settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $adv_parmas = variable_get('gigya_share_advanced', '');
  $form['gigya_share_global']['gigya_advanced_sharing']['gigya_share_advanced'] = array(
    '#type' => 'textarea',
    '#title' => t('Advanced showShareBarUI settings'),
    '#element_validate' => array('gigya_key_val_filed_validate'),
    '#default_value' => !empty($adv_parmas) ? gigya_values_array_to_string($adv_parmas) : '',
    '#description' => t('Use this textbox to enter your own settings. If you are not using this box, make sure it is empty. </br>See list of available !parameters', array('!parameters' => l(t('parameters'), 'http://developers.gigya.com/020_Client_API/020_Methods/socialize.showReactionsBarUI', array('external' => TRUE, 'attributes' => array('target' => '_blank'))))),
  );
  $form['gigya_action_sharing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Action sharing'),
  );
  $form['gigya_action_sharing']['gigya_rules_message'] = array(
    '#markup' => t('Gigya action sharing can be configured with !rules', array('!rules' => l(t('Rules'), 'admin/config/workflow/rules'))),
  );

  return system_settings_form($form);
}

/**
 * Form builder for the Gigya comments administration form.
 *
 * @return array
 *   The Gigya comments administration form.
 */
function gigya_comments_settings_form() {
  $form['gigya_comments_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Gigya Comments'),
    '#default_value' => variable_get('gigya_comments_enabled', TRUE),
  );
  $form['gigya_comments_categoryid'] = array(
    '#type' => 'textfield',
    '#title' => t('Gigya Comments category ID'),
    '#default_value' => variable_get('gigya_comments_categoryid', ''),
    '#description' => t('Category as set up in the Gigya admin site.'),
    '#attributes' => array('placeholder' => 'category1'),
  );
  $form['gigya_comments_container'] = array(
    '#type' => 'textfield',
    '#title' => t('Container ID'),
    '#size' => 30,
    '#description' => t('The container ID that the comments will be displayed in the default is "comments" (Drupal default)'),
    '#default_value' => variable_get('gigya_comments_container', 'comments'),
  );
  $form['gigya_comments_af_scope'] = array(
    '#type' => 'select',
    '#title' => t('Scope for publishing to Activity Feed'),
    '#description' => t('When publishing feed items, by default the feed items are published to social networks only and will not appear on the site\'s Activity Feed plugin. To change this behavior, you must change the publish scope to "Both".'),
    '#options' => array(
      'external' => t('External'),
      'both' => t('Both'),
    ),
    '#default_value' => variable_get('gigya_comments_af_scope', 'external'),
  );
  $form['gigya_comments_hide_drupal_comments'] = array(
    '#type' => 'checkbox',
    '#title' => t('Do not load Drupal comments'),
    '#description' => t('If checked Drupal comment will not load, only Gigya comments. note: this my harm the site SEO as comments will not be indexed by search engines'),
    '#default_value' => variable_get('gigya_comments_hide_drupal_comments', TRUE),
  );

  $form['gigya_comments_advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $comments_extra_parms = variable_get('gigya_comments_extra', array());
  $form['gigya_comments_advanced']['gigya_comments_extra'] = array(
    '#type' => 'textarea',
    '#title' => t('Advanced showCommentsUI settings'),
    '#rows' => 5,
    '#element_validate' => array('gigya_key_val_filed_validate'),
    '#description' => t('Enter values in key|value format. See list of available !parameters', array('!parameters' => l(t('parameters'), 'http://developers.gigya.com/020_Client_API/020_Methods/socialize.showCommentsUI', array('external' => TRUE, 'attributes' => array('target' => '_blank')) ))),
    '#default_value' => !empty($comments_extra_parms) ? gigya_values_array_to_string($comments_extra_parms) : '',
  );

  return system_settings_form($form);
}

/*
 * Reactions form.
 */
function gigya_resctions_settings_form($form, $form_state) {
  $form = array();
  $form['gigya_reaction_message'] = array(
    '#markup' => t('To position a Gigya Reactions plugin, place a "Gigya Reactions" field via the "Structure"/"Content types" menu') . '<br /><br />',
    '#weight' => -50,
  );
  $form['gigya_reactions_global'] = array(
    '#type' => 'fieldset',
    '#title' => t('Gigya reaction global settings'),
    '#collapsible' => FALSE,
  );
  $form['gigya_reactions_global']['gigya_reactions_container_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Container ID'),
    '#size' => 20,
    '#default_value' => variable_get('gigya_reactions_container_id', 'gigya-reactions'),
  );
  $form['gigya_reactions_global']['gigya_reactions_bar_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Bar ID'),
    '#size' => 20,
    '#default_value' => variable_get('gigya_reactions_bar_id', 'reactions-bar'),
  );
  $form['gigya_reactions_global']['gigya_reactions_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Reaction title'),
    '#size' => 60,
    '#default_value' => variable_get('gigya_reactions_title', '[node:title]'),
  );
  $form['gigya_reactions_global']['gigya_reactions_link_back'] = array(
    '#type' => 'textfield',
    '#title' => t('Default linkback URL'),
    '#size' => 60,
    '#default_value' => variable_get('gigya_reactions_title', '[node:url:absolute]'),
  );
  $form['gigya_reactions_global']['gigya_reactions_show_counts'] = array(
    '#type' => 'select',
    '#title' => t('Show counts'),
    '#options' => array(
      'right' => t('Right'),
      'top' => t('Top'),
      'none' => t('None'),
    ),
    '#default_value' => variable_get('gigya_reactions_show_counts', 'right'),
  );
  $form['gigya_reactions_global']['gigya_reactions_layout'] = array(
    '#type' => 'select',
    '#title' => t('Layout'),
    '#options' => array(
      'horizontal' => t('Horizontal'),
      'vertical' => t('Vertical'),
    ),
    '#default_value' => variable_get('gigya_reactions_layout', 'horizontal'),
  );
  $form['gigya_reactions_global']['gigya_reactions_multiple_reactions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use multiple Reactions'),
    '#default_value' => variable_get('gigya_reactions_multiple_reactions', TRUE),
  );
  $form['gigya_reactions_global']['gigya_reactions_count_type'] = array(
    '#type' => 'select',
    '#title' => t('Count type'),
    '#options' => array(
      'number' => t('Number'),
      'percentage' => t('Percentage'),
    ),
    '#default_value' => variable_get('gigya_reactions_count_type', 'number'),
  );
  $form['gigya_reactions_global']['gigya_reactions_af_scope'] = array(
    '#type' => 'select',
    '#title' => t('Scope for publishing to Activity Feed'),
    '#description' => t('When publishing feed items, by default the feed items are published to social networks only and will not appear on the site\'s Activity Feed plugin. To change this behavior, you must change the publish scope to "Both".'),
    '#options' => array(
      'external' => t('External'),
      'both' => t('Both'),
    ),
    '#default_value' => variable_get('gigya_reactions_af_scope', 'external'),
  );
  $form['gigya_reactions_global']['gigya_reactions_image_behavior'] = array(
    '#type' => 'radios',
    '#title' => t('Image'),
    '#options' => array(
      'default' => t('Use image tag if exists, first image on post otherwise.'),
      'first' => t('First image on the post'),
      'url' => t('Specify an image URL'),
    ),
    '#default_value' => variable_get('gigya_reactions_image_behavior', 'default'),
    '#attributes' => array('class' => array('image-behv')),
  );
  $form['gigya_reactions_global']['gigya_reactions_image_url'] = array(
    '#type' => 'textfield',
    '#title' => t('The URL of the image to share'),
    '#size' => 20,
    '#states' => array(
      'visible' => array(
        '.image-behv' => array('value' => 'url'),
      ),
    ),
    '#default_value' => variable_get('gigya_reactions_image_url', ''),
  );
  $form['gigya_reactions_global']['gigya_advanced_reactions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced Reactions settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $adv_parmas = variable_get('gigya_reactions_advanced', '');
  $form['gigya_reactions_global']['gigya_advanced_reactions']['gigya_reactions_advanced'] = array(
    '#type' => 'textarea',
    '#title' => t('Advanced showReactionsBarUI settings'),
    '#element_validate' => array('gigya_key_val_filed_validate'),
    '#default_value' => !empty($adv_parmas) ? gigya_values_array_to_string($adv_parmas) : '',
    '#description' => t('Use this text-box to enter your own settings. If you are not using this box, make sure it is empty. </br> See list of available !parameters', array('!parameters' => l(t('parameters'), 'http://developers.gigya.com/020_Client_API/020_Methods/socialize.showReactionsBarUI', array('external' => TRUE, 'attributes' => array('target' => '_blank'))))),
  );

  return system_settings_form($form);
}

function gigya_gm_settings_form($form, $form_state) {
  $form['gigya_gm_message'] = array(
    '#markup' => t('To position Gigya Gamification plugins, create a Gamification block via the menu below, and then position it via the "Structure"/"Blocks" menu.') . '<br /><br />',
    '#weight' => -50,
  );
  $form['gigya_gm_global'] = array(
    '#type' => 'fieldset',
    '#title' => t('Gigya Gamification global settings'),
    '#collapsible' => FALSE,
  );
  $form['gigya_gm_global']['gigya_gm_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Gigya Gamification'),
    '#default_value' => variable_get('gigya_gm_enable', FALSE),
  );
  $form['gigya_gm_global']['gigya_gm_enable_notifications'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Gamification notifications'),
    '#default_value' => variable_get('gigya_gm_enable_notifications', FALSE),
  );
  $form['gigya_gm_global']['gigya_gm_leaderboard_period'] = array(
    '#type' => 'select',
    '#title' => t('Leaderboard time period'),
    '#options' => array(
      '7day' => t('7 Days'),
      'all' => t('All'),
    ),
    '#default_value' => variable_get('gigya_gm_leaderboard_period', '7day'),
  );
  $form['gigya_gm_global']['gigya_gm_total_count'] = array(
    '#type' => 'textfield',
    '#title' => t('Leaderboard user count'),
    '#size' => 2,
    '#description' => t('Valid values are between 1 to 23'),
    '#default_value' => variable_get('gya_gm_total_count', '12'),
  );
  $form['gigya_gm_blocks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Gigya Gamification blocks'),
    '#collapsible' => FALSE,
  );
  $gm_blocks = variable_get('gigya_gm_blobks', array());
  if (!empty($gm_blocks)) {
    foreach ($gm_blocks as $block_id => $block) {
      $opts[$block_id] = $block['block_name'];
    }
    $form['gigya_gm_blocks']['gigya_gm_delete'] = array(
      '#type' => 'fieldset',
      '#title' => t('Delete blocks'),
      '#collapsible' => FALSE,
      '#weight' => -11,
      '#attributes' => array('class' => array('gm-delete')),
    );
    $form['gigya_gm_blocks']['gigya_gm_delete']['gigya_gm_delete_blocks'] = array(
      '#type' => 'checkboxes',
      '#title' => '',
      '#options' => $opts,
      '#weight' => -10,
    );
    $form['gigya_gm_blocks']['gigya_gm_delete']['gigya_gm_delete_blocks_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#ajax' => array(
        'callback' => 'gigya_gm_delete_block',
        'wrapper' => 'gigya-gm-settings-form',
        'method' => 'html',
        'effect' => 'fade',
      ),
    );
  }
  else {
    $form['gigya_gm_blocks']['gigya_gm_delete']['gigya_gm_delete_blocks'] = array(
      '#markup' => '<div id="gm-delete"></div>',
    );
  }
  $form['gigya_gm_blocks']['gigya_gm_blocks_block_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Block name'),
    '#size' => 60,
  );

  //achievements
  $form['gigya_gm_blocks']['gigya_gm_blocks_achievements'] = array(
    '#markup' => '<span class="label">' . t('Achievements') . '</span>',
    '#prefix' => '<div class="gm-element">',
  );
  $form['gigya_gm_blocks']['gigya_gm_blocks_achievements_enable'] = array(
    '#type' => 'checkbox',
  );
  $achiv_def_id = gigya_gm_default_id($gm_blocks, 'achievements', 'divAchievements');
  $form['gigya_gm_blocks']['gigya_gm_blocks_achievements_div-id'] = array(
    '#type' => 'textfield',
    '#title' => t('Div ID'),
    '#size' => 60,
    '#default_value' => $achiv_def_id,
  );
  $form['gigya_gm_blocks']['gigya_gm_blocks_achievements_weight'] = array(
    '#type' => 'select',
    '#title' => t('Weight'),
    '#options' => drupal_map_assoc(range(-20, 20)),
    '#default_value' => 0,
    '#suffix' => '</div>',
  );

  //challenge status
  $form['gigya_gm_blocks']['gigya_gm_blocks_challenge_status'] = array(
    '#markup' => '<span class="label">' . t('Challenge status') . '</span>',
    '#prefix' => '<div class="gm-element">',
  );
  $form['gigya_gm_blocks']['gigya_gm_blocks_challenge_status_enable'] = array(
    '#type' => 'checkbox',
  );
  $ch_def_id = gigya_gm_default_id($gm_blocks, 'challenge_status', 'divChallengeStatus');
  $form['gigya_gm_blocks']['gigya_gm_blocks_challenge_status_div-id'] = array(
    '#type' => 'textfield',
    '#title' => t('Div ID'),
    '#size' => 60,
    '#default_value' => $ch_def_id,
  );
  $form['gigya_gm_blocks']['gigya_gm_blocks_challenge_status_weight'] = array(
    '#type' => 'select',
    '#title' => t('Weight'),
    '#options' => drupal_map_assoc(range(-20, 20)),
    '#default_value' => 0,
    '#suffix' => '</div>',
  );

  //user status
  $form['gigya_gm_blocks']['gigya_gm_blocks_user_status'] = array(
    '#markup' => '<span class="label">' . t('User status') . '</span>',
    '#prefix' => '<div class="gm-element">',
  );
  $us_def_id = gigya_gm_default_id($gm_blocks, 'user_status', 'divUserStatus');
  $form['gigya_gm_blocks']['gigya_gm_blocks_user_status_enable'] = array(
    '#type' => 'checkbox',
  );
  $form['gigya_gm_blocks']['gigya_gm_blocks_user_status_div-id'] = array(
    '#type' => 'textfield',
    '#title' => t('Div ID'),
    '#size' => 60,
    '#default_value' => $us_def_id,
  );
  $form['gigya_gm_blocks']['gigya_gm_blocks_user_status_weight'] = array(
    '#type' => 'select',
    '#title' => t('Weight'),
    '#options' => drupal_map_assoc(range(-20, 20)),
    '#default_value' => 0,
    '#suffix' => '</div>',
  );

  //Leaderboard
  $form['gigya_gm_blocks']['gigya_gm_blocks_leaderboard'] = array(
    '#markup' => '<span class="label">' . t('Leaderboard') . '</span>',
    '#prefix' => '<div class="gm-element">',
  );
  $form['gigya_gm_blocks']['gigya_gm_blocks_leaderboard_enable'] = array(
    '#type' => 'checkbox',
  );
  $lead_def_id = gigya_gm_default_id($gm_blocks, 'leaderboard', 'divLeaderboard');
  $form['gigya_gm_blocks']['gigya_gm_blocks_leaderboard_div-id'] = array(
    '#type' => 'textfield',
    '#title' => t('Div ID'),
    '#size' => 60,
    '#default_value' => $lead_def_id,
  );
  $form['gigya_gm_blocks']['gigya_gm_blocks_leaderboard_weight'] = array(
    '#type' => 'select',
    '#title' => t('Weight'),
    '#options' => drupal_map_assoc(range(-20, 20)),
    '#default_value' => 0,
    '#suffix' => '</div>',
  );
  $form['gigya_gm_blocks']['gigya_gm_blocks_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create a block'),
    '#ajax' => array(
      'callback' => 'gigya_gm_create_block',
      'wrapper' => 'gigya-gm-settings-form',
      'method' => 'html',
      'effect' => 'fade',
    ),
  );
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'gigya') . '/css/gigya_admin.css',
  );
  $form['#attributes'] = array('class' => 'gigya-gm-settings-form');
  return system_settings_form($form);
}

function gigya_gm_create_block($form, $form_state) {
  $gigya_gm_blobks = variable_get('gigya_gm_blobks', array());
  $values = $form_state['values'];
  //create blocks
  if (!empty($values['gigya_gm_blocks_block_name'])) {
    foreach ($values as $key => $val) {
      if(strpos($key, 'gigya_gm_blocks_') === 0 && $key !== 'gigya_gm_blocks_block_name') {
        $key = str_replace('gigya_gm_blocks_', '', $key);
        $karray = explode('_', $key);
        $prop = end($karray);
        array_pop($karray);
        $comp_key = implode('_', $karray);
        $componentes[$comp_key][$prop] = $val;
      }
    }
    $block_id = str_replace(' ', '_', $values['gigya_gm_blocks_block_name']);
    $gigya_gm_blobks[$block_id] = array(
      'block_name' => $values['gigya_gm_blocks_block_name'],
      'componentes' => $componentes,
    );
  }
  variable_set('gigya_gm_blobks', $gigya_gm_blobks);
  $form_state['input'] = array();
  $form = drupal_rebuild_form('gigya_gm_settings_form', $form_state, $form);
  return $form;
}

function gigya_gm_delete_block($form, $form_state) {
  $gigya_gm_blobks = variable_get('gigya_gm_blobks', array());
  $values = $form_state['values'];
  //delete blocks
  if (!empty($values['gigya_gm_delete_blocks'])) {
    foreach ($values['gigya_gm_delete_blocks'] as $block_id => $val) {
      if (!empty($val)) {
        unset($gigya_gm_blobks[$block_id]);
      }
    }
  }
  variable_set('gigya_gm_blobks', $gigya_gm_blobks);
  $form_state['input'] = array();
  $form = drupal_rebuild_form('gigya_gm_settings_form', $form_state, $form);
  return $form;
}

function gigya_gm_default_id($blocks, $key, $id) {
  $ids = array();
  foreach ($blocks as $block_id => $block) {
    $ids[] = $block['componentes'][$key]['div-id'];
  }
  $last_id = array_search($id, $ids);
  if ($last_id !== FALSE) {
    $parts = explode('-', end($ids));
    if (!empty($parts[1]) && is_numeric($parts[1])) {
      list($seen_id, $i) = $parts;
      $i++;
    }
    else {
      $i = 1;
    }
   $id = $id . '-' . $i;
  }
  return $id;
}

function gigya_activity_feed_settings_form($form, $form_state) {
  $form['gigya_activety_feed_message'] = array(
    '#markup' => t('To position a Gigya Activity Feed plugin, enter the "Structure"/"Blocks" menu and choose where to position the "Gigya Activity Feed" block.') . '<br /><br />',
    '#weight' => -50,
  );
  $form['gigya_activety_feed_global'] = array(
    '#type' => 'fieldset',
    '#title' => t('Gigya Activity Feed settings'),
    '#collapsible' => FALSE,
  );

  $form['gigya_activety_feed_global']['gigya_activety_feed_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Gigya Activity Feed'),
    '#default_value' => variable_get('gigya_activety_feed_enable', FALSE),
  );

  $form['gigya_activety_feed_global']['gigya_activety_feed_container_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Activity Feed container ID'),
    '#size' => 30,
    '#default_value' => variable_get('gigya_activety_feed_container_id', 'ActivityFeedDiv'),
  );
  $form['gigya_activety_feed_global']['gigya_activety_feed_tabs'] = array(
    '#type' => 'textfield',
    '#title' => t('Tabs and order'),
    '#size' => 30,
    '#description' => t('A comma delimited list of tabs names that defines which tabs to show and the tabs order. The optional tabs names are: "everyone", "friends", "me".'),
    '#default_value' => variable_get('gigya_activety_feed_tabs', 'everyone,friends,me'),
  );

  $form['gigya_activety_feed_global']['gigya_activety_feed_size'] = array(
    '#type' => 'checkbox',
    '#title' => t('Set custom size for plugin'),
    '#default_value' => variable_get('gigya_activety_feed_size', FALSE),
  );
  $form['gigya_activety_feed_global']['gigya_activety_feed_width'] = array(
    '#prefix' => '<div class="height-with">',
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#size' => 5,
    '#default_value' => variable_get('gigya_activety_feed_width', '309'),
    '#states' => array(
      'visible' => array(
        '#edit-gigya-activety-feed-size' => array('checked' => TRUE),
      ),
    ),
  );
  $form['gigya_activety_feed_global']['gigya_activety_feed_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#size' => 5,
    '#default_value' => variable_get('gigya_activety_feed_height', '270'),
    '#suffix' => '</div>',
    '#states' => array(
      'visible' => array(
        '#edit-gigya-activety-feed-size' => array('checked' => TRUE),
      ),
    ),
  );
  $form['gigya_activety_feed_global']['gigya_activety_feed_privacy'] = array(
    '#type' => 'select',
    '#title' => t('Activity Feed privacy level'),
    '#description' => t('The privacy level determines how the user action is presented in each of the Activity Feed plugin tabs.'),
    '#options' => array(
      'private' => t('Private'),
      'public' => t('Public'),
      'friends' => t('Friends'),
    ),
    '#default_value' => variable_get('gigya_activety_feed_privacy', 'private'),
  );
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'gigya') . '/css/gigya_admin.css',
  );
  return system_settings_form($form);
}
