(function ($) {
    /**
     * @todo Undocumented Code!
     */
    Drupal.behaviors.gigyaNotifyFriends = {
      attach: function(context, settings) {
        $('.friends-ui:not(.gigyaNotifyFriends-processed)', context).addClass('gigyaNotifyFriends-processed').each(
          function () {
            gigya.services.socialize.getUserInfo({callback:Drupal.gigya.notifyFriendsCallback});
            gigya.services.socialize.addEventHandlers({ onConnect:Drupal.gigya.notifyFriendsCallback, onDisconnect:Drupal.gigya.notifyFriendsCallback});
          }
        );
      }
    };

    /**
     * @todo Undocumented Code!
     */
    Drupal.behaviors.gigyaInit = {
      attach: function(context, settings) {
        if (typeof Drupal.settings.gigya.notifyLoginParams !== 'undefined') {
          Drupal.settings.gigya.notifyLoginParams.callback = Drupal.gigya.notifyLoginCallback;
          gigya.services.socialize.getUserInfo({callback: Drupal.gigya.initResponse});
        }

        // Attach event handlers.
        gigya.socialize.addEventHandlers({onLogin:Drupal.gigya.loginCallback});

        // Display LoginUI if necessary.
        if (typeof Drupal.settings.gigya.loginUIParams !== 'undefined') {
          $.each(Drupal.settings.gigya.loginUIParams, function (index, value) {
            value.context = {id: value.containerID};
            gigya.services.socialize.showLoginUI(value);
          });
        }

        // Display ConnectUI if necessary.
        if (typeof Drupal.settings.gigya.connectUIParams !== 'undefined') {
          gigya.services.socialize.showAddConnectionsUI(Drupal.settings.gigya.connectUIParams);
        }

        // Call ShareUI if it exists.
        if (typeof Drupal.settings.gigya.shareUIParams !== 'undefined') {
          //build a media object
          var mediaObj = {type: 'image', href: Drupal.settings.gigya.shareUIParams.linkBack};
          if ((Drupal.settings.gigya.shareUIParams.imageBhev === 'url') && (Drupal.settings.gigya.shareUIParams.imageUrl !== '')) {
            mediaObj.src = Drupal.settings.gigya.shareUIParams.imageUrl;
          }
          else if (Drupal.settings.gigya.shareUIParams.imageBhev === 'default') {
            if ($('meta[property=og:image]').length > 0) {
              mediaObj.src = $('meta[property=og:image]').attr('content');
            }
            else {
              mediaObj.src = $('#block-system-main img').eq(0).attr('src') || $('img').eq(0).attr('src');
            }
          }
          else {
            mediaObj.src = $('#block-system-main img').eq(0).attr('src') || $('img').eq(0).attr('src');
          }
          // Step 1: Construct a UserAction object and fill it with data.
          var ua = new gigya.services.socialize.UserAction();
          if (typeof Drupal.settings.gigya.shareUIParams.linkBack !== 'undefined') {
            ua.setLinkBack(Drupal.settings.gigya.shareUIParams.linkBack);
          }
          if (typeof Drupal.settings.gigya.shareUIParams.title !== 'undefined') {
            ua.setTitle(Drupal.settings.gigya.shareUIParams.title);
          }
          if (typeof Drupal.settings.gigya.shareUIParams.description !== 'undefined') {
            ua.setDescription(Drupal.settings.gigya.shareUIParams.description);
          }
          ua.addMediaItem(mediaObj);
          var params = {};
          if (typeof Drupal.settings.gigya.shareUIParams.extraParams !== 'undefined') {
            params = Drupal.settings.gigya.shareUIParams.extraParams;
          }
          params.userAction = ua;
          gigya.services.socialize.showShareUI(params);
        }
      }
    };

    /**
     * @todo Undocumented Code!
     */
    Drupal.behaviors.gigyaLogut = {
      attach: function (context, settings) {
        if ($.cookie('Drupal.visitor.gigya') == 'gigyaLogOut') {
          gigya.services.socialize.logout();
          $.cookie('Drupal.visitor.gigya', null);
        }
      }
    };
    /**
     * initiate Gamification
     */

    Drupal.behaviors.gamification = {
      attach: function (context, settings) {
        if (typeof Drupal.settings.gigyaGM !== 'undefined'){
          gigya.services.socialize.getUserInfo({callback: Drupal.gigya.gmInit});
        }
      }
    };
    Drupal.behaviors.GMnotifications = {
      attach: function (context, settings) {
        if (typeof Drupal.settings.gigyaGMnotification !== 'undefined'){
          gigya.services.socialize.getUserInfo({callback: Drupal.gigya.gmNotiInit});
        }
      }
    };
    /**
     * initiate Activity Feed
     */
    Drupal.behaviors.gigyaActivityFeed = {
      attach: function (context, settings) {
        if (typeof Drupal.settings.gigyaActivityFeed !== 'undefined'){
          gigya.socialize.showFeedUI(Drupal.settings.gigyaActivityFeed);
        }
        }
    };
    Drupal.behaviors.addGigyaUidToForm = {
      attach: function (context, settings) {
        if ($('#gigya-link-accounts-form .gigyaUid').length > 0 ) {
          $('#gigya-link-accounts-form .gigyaUid').val(Drupal.gigya.toPost.user.UID);
        }
        if ($('#modal-content form .gigyaUid').length > 0) {
          $('#modal-content form .gigyaUid').val(Drupal.gigya.toPost.user.UID);
        }
        $('#modal-content form .close-modal').once().click( function (e) {
          e.preventDefault();
          Drupal.CTools.Modal.dismiss();
        });
        }
    };
    Drupal.behaviors.gigyaFillRegForm = {
      attach: function (context, settings) {
        if ($('.modal-content #user-register-form').length > 0 ) {
          $('.modal-content #user-register-form input:[name=mail]').val(Drupal.gigya.toPost.user.email);
          $('.modal-content #user-register-form input:[name=name]').val(Drupal.gigya.toPost.user.email);
        }
        }
    };



})(jQuery);

