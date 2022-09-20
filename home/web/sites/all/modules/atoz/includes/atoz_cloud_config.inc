<?php
if (strpos ($_SERVER["SERVER_NAME"], "atoz") !== FALSE) {
  define ('MY_CLOUD_BASE_URL', 'test.atoz.playdynamics.com');
}
else if (strpos ($_SERVER["SERVER_NAME"], "atoz.playdynamics.com") !== FALSE) {
  define ('MY_CLOUD_BASE_URL', 'play.atoz.playdynamics.com');
}
else if (strpos ($_SERVER["SERVER_NAME"], "myanmaronlinecreations") !== FALSE) {
  define ('MY_CLOUD_BASE_URL', 'assets.atoz.myanmaronlinecreations.com');
}
else {
  define ('MY_CLOUD_BASE_URL', 'assets.atoz.com');
}

define ('MY_CLOUD_DEFAULT_BUCKET', MY_CLOUD_BASE_URL);
//
define ('MY_CLOUD_DEBUG_ECHO', false);
define ('MY_CLOUD_DEBUG_ERROR', true);
//
define ('MY_CLOUD_AWS_KEY', 'AKIAIEFSTCF47MJNEZHQ');
define ('MY_CLOUD_AWS_SECRET', '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');
define ('MY_CLOUD_REGION_ENDPOINT', 's3-ap-southeast-1.amazonaws.com');
define ('MY_CLOUD_REGION', 'ap-southeast-1');

// Using aws-sdk-php version 2.88
//
// cd /webbase/peach/web/sites/all/libraries
// mkdir aws_sdk
// cd aws_sdk
// curl -L -O https://github.com/aws/aws-sdk-php/releases/download/2.8.8/aws.zip
// unzip aws.zip

// Create buckets e.g. assets.phoosar.com
// Set CNAME e.g. asets -> assets.phoosar.com.s3-ap-southeast-1.amazonaws.com
