<?php
    global $user;

    if (strpos ($classes, "page-webapp-upload") > 0 || ($user->uid && strpos ($classes, "page-webapp-user"))) {
        $webappScripts = $scripts . '
<script type="text/javascript" src="/sites/all/modules/atoz/js/jquery-1.12.14.min.js"></script>
<script type="text/javascript" src="/sites/all/modules/atoz/js/atoz.js?v1.001"></script>
<script type="text/javascript" src="/sites/all/modules/atoz/js/swiper.min.js"></script>
';
        $webappStyles = '
<link type="text/css" rel="stylesheet" href="/sites/all/modules/plupload/plupload.css" media="all" />
<link type="text/css" rel="stylesheet" href="/sites/all/modules/atoz/css/atoz.css?v1.001" media="all" />
<link type="text/css" rel="stylesheet" href="/sites/all/modules/atoz/css/swiper.min.css" media="all" />

';
    }
    else if ($user->uid) {
        $webappScripts =  $scripts . '
<script type="text/javascript" src="/sites/all/modules/atoz/js/jquery-1.12.14.min.js"></script>
<script type="text/javascript" src="/sites/all/modules/atoz/js/atoz.js?v1.001"></script>
<script type="text/javascript" src="/sites/all/modules/atoz/js/atoz_content.js?v1.001"></script>
<script type="text/javascript" src="/sites/all/modules/atoz/js/swiper.min.js"></script>
';

        if (strpos ($classes, "page-webapp-episode") > 0) {
            $webappScripts .= '
<script type="text/javascript" src="/sites/all/modules/atoz/js/hls.min.js"></script>
';
        }

        $webappStyles = '
<link type="text/css" rel="stylesheet" href="/sites/all/modules/atoz/css/atoz.css?v1.001" media="all" />
<link type="text/css" rel="stylesheet" href="/sites/all/modules/atoz/css/atoz_content.css?v1.001" media="all" />
<link type="text/css" rel="stylesheet" href="/sites/all/modules/atoz/css/swiper.min.css" media="all" />
';
    }
    else {
        $webappScripts = '
<script type="text/javascript" src="/sites/all/modules/atoz/js/jquery-1.12.14.min.js"></script>
<script type="text/javascript" src="/sites/all/modules/atoz/js/atoz.js?v1.001"></script>
<script type="text/javascript" src="/sites/all/modules/atoz/js/swiper.min.js"></script>
';
        $webappStyles = '
<link type="text/css" rel="stylesheet" href="/sites/all/modules/atoz/css/atoz.css?v1.001" media="all" />
<link type="text/css" rel="stylesheet" href="/sites/all/modules/atoz/css/swiper.min.css" media="all" />
';
    }

    $serverName = "http://" . $_SERVER["SERVER_NAME"];

    if (strpos ($classes, "page-webapp-home") && strpos ($_SERVER["SERVER_NAME"], "myhome.com") === false) {
        $postUrl = $serverName . "/webapp";
        $postTitle = htmlentities (ATOZ_SITE_NAME);
        $postImage = $serverName . "/sites/all/modules/atoz/images/logo.png";
        $postDescription = "";

        $description = "";

        $extraHeader = '
<meta property="og:title"         content="'.$postTitle.'" />
<meta property="og:description"   content="'.$postDescription.'" />
<meta property="og:type"          content="website" />
<meta property="og:url"           content="'.$postUrl.'" />
<meta property="og:image"         content="'.$postImage.'" />
<meta property="og:site_name"     content="'.ATOZ_SITE_NAME.'" />
<meta property="fb:app_id"        content="'.MYGM_FACEBOOK_APPID.'"/>
';

	    $facebook = true;
	}
	else {
	    $extraHeader = "";
	    $facebook = false;
	}

    if ($facebook) {
        $fbCode = '
<script>
  window.fbAsyncInit = function() {
    FB.init({
      appId      : "'.MYGM_FACEBOOK_APPID.'",
      xfbml      : false,
      version    : "v2.8"
    });
    FB.AppEvents.logPageView();
  };

  (function(d, s, id){
     var js, fjs = d.getElementsByTagName(s)[0];
     if (d.getElementById(id)) {return;}
     js = d.createElement(s); js.id = id;
     js.src = "//connect.facebook.net/en_US/sdk.js";
     fjs.parentNode.insertBefore(js, fjs);
   }(document, "script", "facebook-jssdk"));
</script>	';

    }
    else {
	    $fbCode = "";
    }

    // don't have the new UA code yet
    if (false && strpos ($_SERVER["SERVER_NAME"], "byscote.com") !== FALSE) {
        $analytics = "
<script>
  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
  })(window,document,'script','https://www.google-analytics.com/analytics.js','ga');

  ga('create', 'UA-87093437-11', 'auto');
  ga('send', 'pageview');

</script>
        ";
    }
    else {
        $analytics = '
<script>
</script>
        ';
    }

?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML+RDFa 1.0//EN"
  "http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language; ?>" version="XHTML+RDFa 1.0" dir="<?php print $language->dir; ?>"<?php print $rdf_namespaces; ?> xmlns:og="http://opengraphprotocol.org/schema/" xmlns:fb="http://www.facebook.com/2008/fbml">

<head profile="<?php print $grddl_profile; ?>">
  <?php print $head; ?>
  <title><?php print $head_title; ?></title>
<?php print $extraHeader; ?>
<link rel="shortcut icon" href="/sites/all/modules/atoz/images/favicon.png" type="image/png" />
<meta name="viewport" content="initial-scale=1.0">
<link type="text/css" rel="stylesheet" href="https://fonts.googleapis.com/css?family=Roboto&amp;oee08v" media="all" />
<?php print $webappStyles; ?>
<?php print $webappScripts; ?>

<?php print $analytics; ?>
</head>
<body class="<?php print $classes; ?>" >
  <?php print $fbCode; ?>
  <?php print $page; ?>
</body>
</html>

