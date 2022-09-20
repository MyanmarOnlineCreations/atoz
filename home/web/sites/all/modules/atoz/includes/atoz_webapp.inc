<?php

function atoz_webapp ($parameter1 = "", $parameter2 = "", $parameter3 = "", $parameter4 = "", $parameter5 = "", $parameter6="") {
    drupal_set_title ("Home");

    $server = $_SERVER["SERVER_NAME"];

    if (strpos ($server, "myberry.com") !== FALSE) {
        $protocol = "http://";
    }
    else if (strpos ($server, "lime.playdynamics.com") !== FALSE) {
        $protocol = "https://";
    }
    else if (strpos ($_SERVER["SERVER_NAME"], "myanmaronlinecreations") !== FALSE) {
        $protocol = "https://";
    }
    else {
        if (strtolower ($_SERVER["REQUEST_SCHEME"]) == "http") {
            $target = "https://" . $server . $_SERVER["REQUEST_URI"];
            drupal_goto ($target);
        }

        $protocol = "https://";
    }

    global $user;

    // if ($user->uid == 0) {
    //     // drupal 7 users don't get sessions (for performance so we use a cookie to store language preferences)

    //     $languageSetting = "";

    //     if (isset ($_COOKIE["language"])) {
    //          $languageSetting = $_COOKIE["language"];
    //     }

    //     // update and drop cookie if the user set the language
    //     if (isset ($_GET["language"])) {
    //         $languageSetting = $_GET["language"];

    //         setcookie("language", $languageSetting,time()+24*60*60*365*10); //10 year cookie
    //     }

    //     if ($languageSetting) {
    //         global $language;

    //         $language->language = $languageSetting;
    //     }

    // }
    // else {
    //     if (in_array('atoz admin', $user->roles)) {
    //         drupal_goto ("atoz");
    //     }
    // }

    $output = "";

    global $user;

    $backbutton = '
    <div class="atoz-back"></div>
    ';
    //return "aa".$parameter1;
    switch ($parameter1) {
        /*case "splash":
            require_once ("atoz_splash.inc");
            return atoz_webapp_splash ($parameter2, $parameter3, $parameter4);
            break;
        case "signup":
            require_once ("atoz_signup.inc");
            $resultBlock =  atoz_webapp_signup ($parameter2, $parameter3, $parameter4);
            break;
        case "user":
            require_once ("atoz_user.inc");
            $resultBlock = atoz_webapp_user ($parameter2, $parameter3, $parameter4);
            break;
        case "info":
            require_once ("atoz_info.inc");
            $resultBlock = atoz_webapp_info ($parameter2, $parameter3, $parameter4);
            break;

        case "terms":
            require_once ("atoz_terms.inc");
            $resultBlock = atoz_webapp_terms ($parameter2, $parameter3, $parameter4);
            break;
        case "test42":
            require_once ("atoz_test42.inc");
            $resultBlock = atoz_test42 ($parameter2, $parameter3, $parameter4);
            break;

        case "signin": // this is for testing only
            require_once ("atoz_user.inc");
            $resultBlock = atoz_webapp_user_signin ($parameter2, $parameter3, $parameter4);
            break;
        case "ajax": // this is for testing only
            require_once ("atoz_ajax.inc");
            $resultBlock = atoz_atoz_ajax ($parameter2);
            break;
        case "total_rating" : 
            require_once ("atoz_ajax.inc");
            $resultBlock = atoz_atoz_ajax_total($parameter2);
            break;
        
/* not needed anymore
        case "validate_user": // this is for testing only
            require_once ("atoz_user.inc");
            $resultBlock = atoz_webapp_validate_user ($parameter2, $parameter3, $parameter4);
            break;
        case "atozgabar":
            require_once ("atoz_user.inc");
            $resultBlock = atoz_webapp_redirect_to_atozgabar ($parameter2, $parameter3, $parameter4);
            break;
*/      case "print":
            require_once("atoz_print_invoice.inc");
            return atoz_print_invoice($parameter2);
            break;
        case "make_payment":
            require_once("atoz_print_invoice.inc");
            return atoz_payment($parameter2);
            break;
        case "approved_meter_unit":
            require_once ("atoz_ajax.inc");
            return myatoz_approved_meter_unit ();
            break;
        default:
            require_once ("atoz_content.inc");
            $resultBlock = atoz_webapp_content ($parameter1, $parameter2, $parameter3, $parameter4);

            if ($user->uid) {
                $backbutton = '';
            }
            else {
                $backbutton = '
        <a href="/user/simple-fb-connect?destination=/webapp/user" />
            <div class="atoz-facebook-signin">
            <img src="./sites/all/modules/atoz/images/facebook_icon.png" />'.t('login').'
            </div>
        </a>';
            }
            break;
    }

    $errorBlock =  atoz_webapp_checkerrors ();

    $debug = MYatoz_DEBUG_MODE;

    if ($user->uid) {
        $menuLast = 'atoz-menu-last';

        $menuAccount = '
          <a href="/user/logout?destination=/webapp"><div class="atoz-menu-item atoz_menu_account">' . t('Logout') . '</div></a>
        ';
    }
    else {
        $menuAccount = '
        <a href="/webapp/signup" />
            <div class="atoz-menu-item atoz_menu_account">'.t('Sign up with Facebook').'
            </div>
        </a>        ';
        $menuLast = '';
    }


    $menu = '
    <div id="atoz-menu-block" class="atoz-menu-block">
        <div id="atoz-menu" class="atoz-menu"></div>
        <div class="atoz-menu-overlay">
            <div class="atoz-menu-inner-block">
                <div class="atoz-menu-items">
                    <a href="/webapp"><div class="atoz-menu-item atoz-menu-home">' . t('atoz') . '</div></a>
                    ' . $menuAccount . '
                    <a href="/webapp/category/"><div class="atoz-menu-item atoz_menu_category">' . t('Categories') . '</div></a>
                </div>
                <div class="atoz-menu-close">
            </div>
            </div>
        </div>
    </div>
';

	$output = $errorBlock . '
<div id="webapp-wrapper">
    ' . $menu . '
	<div id="webapp-content" class="webapp-content-'.$parameter1 . '" >
        <div class="content-top">
            <a href="/webapp">
            <div class="atoz-content-logo">
                <img src="./sites/all/modules/atoz/images/webapp_logo.png" />
            </div>
            </a>
        ' . $backbutton . '
        </div>
        <div class="content-block">
        ' . $resultBlock . '
        </div>
    </div>
</div>
<div id="webapp-footer">
&copy; 2018 myanmaronlinecreations.com
</div>
';
	return $output;
}

//print invoice 





