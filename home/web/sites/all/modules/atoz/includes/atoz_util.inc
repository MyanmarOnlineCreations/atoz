<?php


define ('ATOZ_MEMBERSHIP_TRIAL', '0');
define ('ATOZ_MEMBERSHIP_PAID', '1');

define ('ATOZ_NOT_EXPIRED', '0');
define ('ATOZ_WILL_EXPIRE', '1');
define ('ATOZ_EXPIRED', '2');

function atoz_webapp_checkerrors ($defaultMessage = "", $hide = false, $okMessage = "ok") {

    if ($okMessage == "ok") {
        $okMessage = t('OK');
    }

    if ($okMessage === NULL) {
        $okDiv = "";
    }
    else {
        $okDiv = '<div class="atoz-error-ok">' . $okMessage . '</div>';
    }


    $errorBlock = "";
    $message = $defaultMessage;

	if (isset($_SESSION["atoz_error"])) {
	    $message = $_SESSION["atoz_error"];
    }

	if ($message) {
	    if ($hide) {
	        $style = 'style="display: none"';
	    }
	    else {
	        $style = '';
	    }

		$errorBlock = '
<div id="atoz-error-block" ' . $style .'>
	<div class="atoz-error-background"></div>
	<div class="atoz-error-message">' .$message .'</div>
	' . $okDiv . '
</div>
	';
		unset ($_SESSION["atoz_error"]);
	}

	return $errorBlock;

}

function atoz_web_url ($uri) {
    $webUri = str_replace ("public://", "/sites/default/files/", $uri);

    return $webUri;
}


function atoz_full_file_path($fileName, $uri='public://') {
	// return the full path to the file
	$path = '';
	if ($wrapper = file_stream_wrapper_get_instance_by_uri($uri)) {
		$path = $wrapper->realpath() . '/' . $fileName;
	}
	return $path;
}

function atoz_temp_path() {
    return variable_get('file_default_scheme', 'public') . '://temp/';
}

function atoz_file_path() {
    //return variable_get('file_default_scheme', 'public') . '://social/images/';
    return variable_get('file_default_scheme', 'public') . '://'.$type.'/';
}

function atoz_profile_path() {
    return variable_get('file_default_scheme', 'public') . '://social/profiles/';
}


function atoz_get_path ($type) {

    $path = atoz_file_path ($type);
    return $path;
}

function atoz_web_path ($type = "images") {
    $path = "";

    if (ATOZ_LOCAL_FILESTORAGE) {
       $path =  '/sites/default/files/atoz/'.$type.'/';
    }
    else {
        // S3 CODE
        require_once ("atoz_cloud.inc");
        $mycloud = new ATOZCloud();
        $path = $mycloud->file_url('/atoz/'.$type.'/');
    }
    return $path;

}

function atoz_web_profile_path () {
    $path = "";

    if (ATOZ_LOCAL_FILESTORAGE) {
       $path =  '/sites/default/files/social/profiles/';
    }
    else {
        // S3 CODE
        require_once ("atoz_cloud.inc");
        $mycloud = new ATOZCloud();
        $path = $mycloud->file_url('/social/profiles/');
    }
    return $path;

}


//  newly added for category folder page
function atoz_category_image_path () {
    $path = "";

    if (ATOZ_LOCAL_FILESTORAGE) {
       $path =  '/sites/default/files/social/category/';
    }
    else {

        require_once ("atoz_cloud.inc");
        $mycloud = new ATOZCloud();
        $path = $mycloud->file_url('/social/category/');
    }
    return $path;

}

//  newly added for application folder page
function atoz_application_path () {
    $path = "";

    if (ATOZ_LOCAL_FILESTORAGE) {
       $path =  '/sites/default/files/social/application/';
    }
    else {
        require_once ("atoz_cloud.inc");
        $mycloud = new ATOZCloud();
        $path = $mycloud->file_url('/social/application/');
    }
    return $path;

}

// newly added for atoz_img folder page
function atoz_atozimage_path () {
    $path = "";
    if(ATOZ_LOCAL_FILESTORAGE)
    {
       $path = '/sites/default/files/social/atoz_img/';
    }else{
       require_once ("atoz_cloud.inc");
       $mycloud = new ATOZCloud();
       $path = $mycloud->file_url('/social/atoz_img/');
    }
}

function atoz_save_profile_picture ($picture_url) {
    $file = new stdClass ();

    $cleanSourceParts = explode ("?", $picture_url);

    $extension = pathinfo ($cleanSourceParts[0], PATHINFO_EXTENSION);

    $profileImage = atoz_get_unique_image_name ($extension);

    $file->filename = $profileImage;

    $source = atoz_temp_path () . $profileImage;

    $tempFile = system_retrieve_file($picture_url, $source, FALSE, FILE_EXISTS_REPLACE);

    if ($tempFile) {

        $destination = atoz_save_file ($source, $profileImage, FILE_EXISTS_REPLACE, "profile");

        if ($destination) {
            $file->uri = $destination;
            $file->status = FILE_STATUS_PERMANENT;
            $file = file_save($file);
        }
    }

    return $file;
}

function atoz_save_file ($fileSource, $filename,  $action = FILE_EXISTS_RENAME, $type ="file") {
    //print_r($fileSource."<br/>".$filename."<br/>".$action."<br/>".$type);
    //$result = true;
    // ATOZ_LOCAL_FILESTORAGE set to 0 on live server so it goes to S3
    //echo "result = " . $result;

    if (ATOZ_LOCAL_FILESTORAGE) {
       $result =  atoz_save_local_file ($fileSource, $filename, $action, $type);
    }
    else {
        // S3 CODE
        echo "staging file ";
       $result =  atoz_save_cloud_file ($fileSource, $filename, $action, $type);
    }

    return $result;
}

function atoz_save_local_file ($fileSource, $filename, $action = FILE_EXISTS_REPLACE, $type = "file") {
    //print_r($fileSource."<br/>".$filename."<br/>".$action."<br/>".$type . "testtest<br/>");

    /*if ($type == "profile") {
        $path = atoz_profile_path ();
   }
    else if ($type == "category") {

       $path = atoz_category_path();
   }
   else if ($type == "screenshot"){
       $path = atoz_screenshot_path();
   }
   else {
        $path = atoz_file_path ();
   }*/
    //$path = atoz_get_path ($type);
    //print_r($path ."<br/>");
    $path = atoz_get_path ($type);
    $destination = $path . $filename;
    //print_r($destination . "<br/>");

    $finalDestination = file_unmanaged_move($fileSource, $destination, $action);

   // print_r($finalDestination . '<br/>');

    return $finalDestination;
}

function atoz_delete_file ($filename, $type = "file") {
    $result = true;

    // ATOZ_LOCAL_FILESTORAGE set to 0 on live server so it goes to S3

    if (ATOZ_LOCAL_FILESTORAGE) {
        $result = atoz_delete_local_file ($filename, $type);
    }
    else {
        // S3 CODE
        $result = atoz_delete_cloud_file ($filename, $type);
    }

    return $result;
}

function atoz_delete_local_file ($filename, $type = "file") {
    $path = atoz_get_path ($type);

    $picturePath = $path . $filename;
    file_unmanaged_delete ($picturePath);

    return true;
}

function atoz_file_default_scheme_for_cloud($path) {
    // should match atoz_file_path() and atoz_profile_path()
    $search = variable_get('file_default_scheme', 'public') . ':/';
    return str_replace($search, '', $path);
}

// based on file_create_filename()
function atoz_file_create_cloud_filename($basename, $directory, $fileExists) {
  // Strip control characters (ASCII value < 32). Though these are allowed in
  // some filesystems, not many applications handle them well.
  $basename = preg_replace('/[\x00-\x1F]/u', '_', $basename);
  if (substr(PHP_OS, 0, 3) == 'WIN') {
    // These characters are not allowed in Windows filenames
    $basename = str_replace(array(':', '*', '?', '"', '<', '>', '|'), '_', $basename);
  }

  // A URI or path may already have a trailing slash or look like "public://".
  if (substr($directory, -1) == '/') {
    $separator = '';
  }
  else {
    $separator = '/';
  }

  $destination = $directory . $separator . $basename;

  if ($fileExists) {
    // Destination file already exists, generate an alternative.
    $pos = strrpos($basename, '.');
    if ($pos !== FALSE) {
      $name = substr($basename, 0, $pos);
      $ext = substr($basename, $pos);
    }
    else {
      $name = $basename;
      $ext = '';
    }

    $counter = mt_rand(1000,9999);
    $destination = $directory . $separator . $name . '_' . $counter . $ext;
  }

  return $destination;
}

function atoz_save_cloud_file ($fileSource, $filename, $action = FILE_EXISTS_RENAME, $type = "file") {

    /*if ($type == "profile") {
        $path = atoz_profile_path ();
   }
    else if ($type == "category") {
        echo "reach category";

       $path = atoz_category_path();
   }
   else if ($type == "screenshot"){
       $path = atoz_screenshot_path();
   }
   else {
        $path = atoz_file_path ();
   }
    echo "path =" . $path;
    // $path = atoz_get_path ($type);*/

    $path = atoz_get_path ($type);

    require_once ("atoz_cloud.inc");
    $mycloud = new ATOZCloud();

    $path = atoz_file_default_scheme_for_cloud($path);
    $destination = $path . $filename;

    if (FILE_EXISTS_ERROR == $action) {
        // if file exists then return FALSE, else continue
        if ($mycloud->does_file_exists($destination)) {
            return FALSE;
        }
    } else
    if (FILE_EXISTS_RENAME == $action) {
        // if file exists then generate a new name and continue
        $safety = 0;
        while ($mycloud->does_file_exists($destination) && $safety++ < 10) {
            $basename = drupal_basename($destination);
            $directory = drupal_dirname($destination);
            $destination = atoz_file_create_cloud_filename($basename, $directory, $fileExists=true);
        }
    }
    //$mycloud->log("atoz_save_cloud_file: fileSource:$fileSource,destination:$destination");

    $result = $mycloud->upload_file($fileSource, $destination);
    if ($mycloud->call_succeeded($result)) {
        //$mycloud->log("atoz_save_cloud_file: SUCCEEDED result:".print_r($result,true));
        $finalDestination = $result['MSCloudUrl'];
    } else {
        $mycloud->log("atoz_save_cloud_file: FAILED: result:".print_r($result,true));
        $finalDestination = FALSE;
    }

    return $finalDestination;
}

function atoz_delete_cloud_file ($filename, $type = "file") {
    $path = atoz_get_path ($type);

    require_once ("atoz_cloud.inc");
    $mycloud = new ATOZCloud();

    $path = atoz_file_default_scheme_for_cloud($path);
    $destination = $path . $filename;

    $result = $mycloud->remove_file($destination);
    if ($mycloud->call_succeeded($result)) {
        $mycloud->log("atoz_delete_cloud_file: SUCCEEDED");
    } else {
        $mycloud->log("atoz_delete_cloud_file: FAILED");
    }

    return true;
}


function atoz_remove_user ($userID = null) {
    global $user;

    $deleteUID = $user->uid;

    $selfDelete = true;

    // only admins can delete other users
    if ($userID) {
        if (in_array('dating admin', $user->roles)) {
            $deleteUID = $userID;

            $selfDelete = false;
        }
        else { // profile_id specified but not admin
            return;
        }
    }

    if ($deleteUID > 1000) {
        $account = user_load ($deleteUID);

         // delete profile picture
        $profilePicture = null;

        if ($account->picture) {
            if (is_numeric($account->picture)) {
                $account->picture = file_load($account->picture);
            }

            $profilePicture = $account->picture->filename;
        }

        if ($profilePicture) {
            // delete again because it may be on S3
            atoz_delete_file ($profilePicture, "profile");
        }

        $query =  db_select ("atoz_user_info", "i");
        $query->join ("users", "u", "u.uid = i.uid");
        $query->condition("i.uid", 1000, ">");
        $query->condition("i.uid", $deleteUID);
        $query->fields ("u", array ("created"));
        $query->fields ("i", array ("uid", "gender", "min_age", "max_age", "email", "facebook_id"));
        $result = $query->execute ();

        $deleteTime = time();

        foreach ($result as $row) {
            db_insert ("atoz_user_deleted")
                ->fields (array (
                    "uid"           => $deleteUID,
                    "gender"        => $row->gender,
                    "min_age"       => $row->min_age,
                    "max_age"       => $row->max_age,
                    "email"         => $row->email,
                    "facebook_id"   => $row->facebook_id,
                    "delete_time"   => $deleteTime,
                    "created"       => $row->created
                ))
                ->execute ();
            break;
        }

        $result = db_update ("users")
            ->condition ("uid", $deleteUID)
            ->fields (array ("status"=>0))
            ->execute ();

        // mark it for delete in case someone else is matching to this
        $result = db_update ("atoz_user_info")
            ->condition ("uid", $deleteUID)
            ->fields (array ("deleted"=> 1))
            ->execute ();

        db_delete ("atoz_user_info")
            ->condition ("uid", $deleteUID)
            ->execute ();

        db_delete ("users")
            ->condition ("uid", $deleteUID)
            ->execute ();

        if ($selfDelete) drupal_goto ("webapp/thank_you/goodbye");
    }
    else {
        if ($selfDelete) drupal_goto ("webapp/home");
    }
}

function atoz_check_for_deleted_users () {
    $count = 0;

    $query =  db_select ("atoz_user_info", "i");
    $query->join ("users", "u", "u.uid = i.uid");
    $query->condition("i.uid", 1000, ">");
    $query->condition("deleted", "1");
    $query->fields ("u", array ("created"));
    $query->fields ("i", array ("uid", "gender", "min_age", "max_age", "email", "facebook_id"));
    $result = $query->execute ();

    $deleteTime = time();

    foreach ($result as $row) {
        $deleteUID = $row->uid;

        echo  $deleteUID . " (" . $row->deleted. ")<br />";

        $count++;

        db_insert ("atoz_user_deleted")
            ->fields (array (
                "uid"           => $deleteUID,
                "gender"        => $row->gender,
                "min_age"       => $row->min_age,
                "max_age"       => $row->max_age,
                "email"         => $row->email,
                "facebook_id"   => $row->facebook_id,
                "delete_time"   => $deleteTime,
                "created"       => $row->created
            ))
            ->execute ();

        if ($deleteUID > 1000 && $row->deleted == 1) { // let's be a little paranoid
            echo "> deleting " . $deleteUID . "...<br />";

            db_delete ("atoz_user_info")
                ->condition ("uid", $deleteUID)
                ->execute ();

            db_delete ("users")
                ->condition ("uid", $deleteUID)
                ->execute ();
        }
    }

    echo $count . " accounts deleted<br />";
}

function atoz_get_unique_image_name ($extension) {
    $retry = 10;

    $profileImage = "";

    while ($retry) {
        $uniqueID = uniqid ("U", true);

        $basename = str_replace (".", "_", $uniqueID);

        $profileImage = $basename . "." . $extension;

        $query = db_select ("file_managed", "f")
            ->condition("filename", $profileImage)
            ->fields ("f", array("fid"))
            ->range (0, 1);

        $result = $query->execute();

        $found = false;

        foreach ($result as $row) {
            $found = true;
            break;
        }

        if (!$found) break; // it's unique

        $retry--;

        // no more retries so just add a timestamp to the end to make it unique
        if (!$retry) $profileImage = $basename . "_" . time() . "." . $extension;
    }

    return $profileImage;
}

function atoz_get_unique_profile_id ($prefix = 'U') {
    $retry = 10;

    $uniqueID = "";

    while ($retry) {
        if ($prefix == "O") {

            $uniqueID = uniqid ("O", true);

            $query = db_select ("tbl_home_outlets", "o")
                ->condition("goid", $uniqueID)
                ->fields ("o", array("goid"))
                ->range (0, 1);
        }else if ($prefix == "M") {

            $uniqueID = uniqid ("M", true);

            $query = db_select ("tbl_home_members_info", "m")
                ->condition("gmid", $uniqueID)
                ->fields ("m", array("gmid"))
                ->range (0, 1);
        }else if ($prefix == "R") {

            $uniqueID = uniqid ("R", true);

            $query = db_select ("tbl_home_rules", "r")
                ->condition("ruleId", $uniqueID)
                ->fields ("r", array("ruleId"))
                ->range (0, 1);
        }else{

            $uniqueID = uniqid ("U", true);

            $query = db_select ("tbl_home_users_info", "i")
                ->condition("guid", $uniqueID)
                ->fields ("i", array("guid"))
                ->range (0, 1);
        }
        

        $result = $query->execute();

        $found = false;

        foreach ($result as $row) {
            $found = true;
            break;
        }

        if (!$found) break; // it's unique

        $retry--;

        // no more retries so just add a timestamp to the end to make it unique
        if (!$retry) $uniqueID = $uniqueID . "." . time();
    }

    return $uniqueID;
}

function atoz_get_unique_giftitem_id($prefix = 'G') {
    $retry = 10;

    $uniqueID = "";

    while ($retry) {
        $uniqueID = uniqid ("G", true);

        $query = db_select ("tbl_home_gift", "g")
            ->condition("grid", $uniqueID)
            ->fields ("g", array("grid"))
            ->range (0, 1);
       
        $result = $query->execute();

        $found = false;

        foreach ($result as $row) {
            $found = true;
            break;
        }

        if (!$found) break; // it's unique

        $retry--;

        // no more retries so just add a timestamp to the end to make it unique
        if (!$retry) $uniqueID = $uniqueID . "." . time();
    }

    return $uniqueID;
}
function atoz_get_user_info ($profileID, $uid = null) {

    global $user;

    $data = array();
    $data["gender"]     = "";
    $data["min_age"]    = "";
    $data["max_age"]    = "";
    $data["email"]      = "";
    $data["profile_id"] = "";
    $data["picture"]    = "";
    $data["firstname"]  = "";
    $data["lastname"]   = "";
    $data["profile_image"] = null;

    $profileImage = null;

    $query = db_select("atoz_user_info", "i");
    $query->join("users", "u", "u.uid = i.uid");
    $query->leftjoin ("file_managed", "f", "f.fid = u.picture");
    $query->fields ("i");
    $query->fields ("u", array("picture"));
    $query->fields ("f", array("uri", "timestamp"));

    if ($uid) {
        $query->condition("i.uid", $uid);
    }
    else if ($profieID) {
        $query->condition("i.profile_id", $profileID);
    }

    $query->condition("u.status", "1");

    $result = $query->execute ();

    $picture = 0;

    foreach ($result as $row) {
        $data["min_age"]    = $row->min_age;
        $data["max_age"]    = $row->max_age;
        $data["email"]      = $row->email;
        $data["firstname"]  = $row->firstname;
        $data["lastname"]   = $row->lastname;
        $data["uid"]        = $row->uid;
        $data["profile_id"] = $row->profile_id;

        if ($row->uri) {
            $data["profile_image"] = atoz_web_url ($row->uri) . "?v=" . $row->timestamp;
        }
        else {
            $data["profile_image"] = null;
        }
    }

    return $data;
}

function check_profile_ids () {
    $result = db_select ("atoz_user_info", "i")
        ->isnull ("i.profile_id")
        ->fields ("i", array ("uid"))
        ->execute ();

    foreach ($result as $row) {
        $profileID = atoz_get_unique_profile_id ();

       db_merge ("atoz_user_info")
        ->key(array ("uid"=>$row->uid))
        ->fields (array("profile_id"=>$profileID))
        ->execute ();
    }

    return;
}

function atoz_NumberOfSetBits($v)
{
    $c = $v - (($v >> 1) & 0x55555555);
    $c = (($c >> 2) & 0x33333333) + ($c & 0x33333333);
    $c = (($c >> 4) + $c) & 0x0F0F0F0F;
    $c = (($c >> 8) + $c) & 0x00FF00FF;
    $c = (($c >> 16) + $c) & 0x0000FFFF;
    return $c;
}

function atoz_convertToLocalTime ($date) {
	return date ("Y-m-d G:i:s", strtotime($date . " UTC"));
}

function atoz_convertToLocalDate ($date) {
	return date ("Y-m-d", strtotime($date . " UTC"));
}

function atoz_convertToLocalTimeFromTimeStamp ($timeStamp) {
	return date ("Y-m-d G:i:s", $timeStamp);
}

function atoz_curl ($url, &$error, &$errorno, &$httpStatus) {
    return atoz_curl_ex ($url, false, null, $error, $errorno, $httpStatus);
}

function atoz_curl_ex ($url, $post, $postFields, &$error, &$errorno, &$httpStatus) {

	// create a new cURL resource
	$ch = curl_init();

	// set URL and other appropriate options

	curl_setopt($ch, CURLOPT_URL, $url);
    if (strpos ($_SERVER["SERVER_NAME"], "mylime.com") !== FALSE) {
    	curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 1);
    }
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
	curl_setopt($ch, CURLOPT_FOLLOWLOCATION, false);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 30);
    curl_setopt($ch, CURLOPT_TIMEOUT, 30);

    if ($post) {
         curl_setopt($ch,CURLOPT_POST,true);
         curl_setopt($ch,CURLOPT_POSTFIELDS,$postFields);
    }

	// grab URL and pass it to the browser
	$response = curl_exec($ch);

    $httpStatus = curl_getinfo($ch, CURLINFO_HTTP_CODE);
    $lastEffectiveUrl = curl_getinfo($ch, CURLINFO_EFFECTIVE_URL);

	$error = curl_error ($ch);
	$errorno = curl_errno ($ch);

	if ($error) {
	    error_log ("Curl error: " . $errorno . ": " . $error . " " . $url);
	}

	if ($httpStatus < 200 || $httpStatus > 299) {
	    error_log ("Curl status: httpStatus - " . $httpStatus . " " . $errorno . ": " . $error . " " . $url);
	}

	// close cURL resource, and free up system resources
	curl_close($ch);

	return $response;
}

function atoz_set_expiry (&$expiry, &$membershipType) {
    $expiry = 0;
    $membershipType = atoz_MEMBERSHIP_TRIAL;

    global $user;

    $promotionTime = strtotime("2017-04-13 00:00:00");
    $promotionTime2 = strtotime("2017-05-05 00:00:00");

    $conditional2 = "if(created < '".$promotionTime2."', UNIX_TIMESTAMP(date_add(from_unixtime(created), interval 2 month)), UNIX_TIMESTAMP(date_add(from_unixtime(created), interval 1 month)))";
    $conditional = "if(created < '".$promotionTime."', UNIX_TIMESTAMP(date_add(from_unixtime(created), interval 3 month)), ". $conditional2 .")";

    global $user;

    $query = db_select ("users", "u");
    $query->condition ("u.uid", $user->uid);
    $query->addexpression ($conditional, "expiry_date");
    $query->fields ("u", array ("created"));
    $result = $query->execute ();

    $creationTime = "";
    foreach ($result as $row) {
        $creationTime = $row->created;
        $expiry = $row->expiry_date;
    }

    // $joinDate = date("j M Y", $creationTime);

    // this is complicated
    // if the payment date is less than the expiry we add the month to the expiry
    // if the payment date is greater than the expiry then we add the month to th e payment date

    $query = db_select ("atoz_transactions", "t");
    $query->join("atoz_user_info", "i", "i.profile_id = t.profile_id");
    $query->fields ("t", array ("amount", "posting_date"));
    $query->addexpression ("UNIX_TIMESTAMP(posting_date)", "posting_time");
    $query->condition ("i.uid", $user->uid);
    $result = $query->execute ();

    $total = 0;

    $monthSubscription = 2000;
    $extra = 0;

    foreach ($result as $row) {
        $amount = $row->amount;
        $total += $amount;

        $amount += $extra; // if they overpay the multiples
        $monthsToAdd = (int) ($amount / $monthSubscription);

        $extra = $amount - $monthsToAdd * $monthSubscription;

        $postingDate = $row->posting_date;
        $postingTime = $row->posting_time;

        // account expired in the past and then they paid so start is from posting date
        if ($postingTime > $expiry) {
            $expiry = $postingTime;
        }

        // calculate new expiry time

        $dateObj = new DateTime ();
        $dateObj->setTimestamp ($expiry);
        $dateObj->modify ('+'.$monthsToAdd.' month');

        $expiry = $dateObj->getTimestamp();
    }

    if ($total) $membershipType = atoz_MEMBERSHIP_PAID;

    $fields = array (
        "expiry" => $expiry,
        "membership_type" => $membershipType
    );

    $query = db_update ("atoz_user_info");
    $query->fields ($fields);
    $query->condition ("uid", $user->uid);
    $result = $query->execute ();

    $query = db_delete ("atoz_user_notification");
    $query->condition ("uid", $user->uid);
    $query->execute();

    setcookie("atoz_will_expire", "show", 1, "/"); //10 year cookie
    setcookie("atoz_expired", "show", 1, "/"); //10 year cookie
}

function atoz_check_expiry ($membershipType, $expiry, &$expiryMessage) {
    $message = "";
    $expiryType = atoz_NOT_EXPIRED;

    $now = time ();

    $debugExpired = 0;

    if (isset ($_GET["expired"])) {
        $debugExpired = $_GET["expired"];
    }

    if ($expiry < $now || $debugExpired > 7) {
        $expiryType = atoz_EXPIRED;

        if ($membershipType == atoz_MEMBERSHIP_PAID) {
            $message = t('Your membership has expired. Go to the setting page in the EZdin-gar section to renew your account.');
        }
        else {
            $message = t('Your free trial membership has expired. Please go to the settings page in the EZdin-gar section to find out how to renew your account.');
        }
    }
    else if ($expiry < ($now + 604800) || $debugExpired > 0) {
        $expiryType = atoz_WILL_EXPIRE;

        $daysLeft = (int) (($expiry - $now) / 86400) + 1;

        if ($debugExpired) $daysLeft = $debugExpired;

        switch ($daysLeft) {
            case 1:
                $timeLeft = "one day.";
                break;
            case 2:
                $timeLeft = "two days.";
                break;
            case 3:
                $timeLeft = "three days.";
                break;
            case 4:
                $timeLeft = "four days.";
                break;
            case 5:
                $timeLeft = "five days.";
                break;
            case 6:
                $timeLeft = "six days.";
                break;
            default:
                $timeLeft = "one week.";
                break;
        }

        if ($$memberType == atoz_MEMBERSHIP_PAID) {
            $message1 = t('Please note your monthly membership is going to expire in ');
            $message2 = t($timeLeft);
            $message3 = ' ' . t(' Go to the setting page in the EZdin-gar section to renew your account.');

            $message = $message1 . $message2 . $message3;
        }
        else {
            $message1 = t('Your free trial membership is going to expire in ');
            $message2 = t($timeLeft);
            $message3 = ' ' . t('Please go to the settings page in the EZdin-gar section to find out how to renew your account.');

            $message = $message1 . $message2 . $message3;
        }
    }

//    $membership = date("j M Y", $lastExpiryTime);

    $expiryMessage = $message;

    return $expiryType;
}

// use this to create a key to ensure url is not spoofed

function atoz_create_key ($profileID, &$time = 0) {

    if (!$time) $time = time (); // unix timetamp

    return md5 ($profileID . $time . "M8bif3bBiz8QDb82aHGp");
}

// validate the url is current and correct
// we check both the key and time i.e. url is only valid for 10 min
// generally sign requests are in seconds so that be more than enough
// time validity is to stop people from bookmarking or sharing the url
// if it is known

function atoz_validate_key($profileID, $time, $key, &$errorCode) {

    $myKey = atoz_create_key ($profileID, $time);

    $myTime = time (); // unix timestamp

    // check if key matches and time is within 10 minutes

    if ($key != $myKey) $errorCode = -1;
    else if (($myTime - $time) > 600) $errorCode = -2;
    else $errorCode = 0;

    if ($errorCode == 0) {
        return true;
    }
    else {
        return false;
    }
}

// 0: no error
// -99: no data found in POST request
// -1: invalid key
// -2: request is more than 10 min old
// -3: invalid MOC id

function atoz_validate_user () {
    $errorCode = -4;

    $mid = ""; // moc user profile id
    $gid = ""; // atozgabar id
    $expiryDate = "";
    $expiryTime = "";
    $tid = "";
    $key = "";
    $rid = "0";

    if (isset ($_POST["data"])) {

        $data = json_decode ($_POST["data"]);

        $rid = 1;

        if (isset($data->mid)) $mid = $data->mid;
        if (isset($data->rid)) $rid = $data->rid;
        if (isset($data->gid)) $gid = $data->gid;
        if (isset($data->tid)) $tid = $data->tid;
        if (isset($data->key)) $key = $data->key;

       if (atoz_validate_key ($mid, $rid, $key, $errorCode)) {
            $userInfo = atoz_get_user_info ($mid);

            if ($userInfo["uid"]) {
                // hardcode expiry until end of each month for now

                date_default_timezone_set ("Asia/Rangoon");

                $expiryDate = date ("Y-m-t");

                $expiryTime = strtotime ($expiryDate) + 24 *3600;

                $gidData = substr ($gid, 0, 40);
                $tidData = substr ($tid, 0, 255);

                $query = db_update ("atoz_user_info")
                    ->fields (array ("atoz_id"=>$gidData))
                    ->condition ("profile_id", $mid)
                    ->execute ();

                $errorCode = 0;
            }
            else {
                $errorCode = -3;
            }
        }
    }

    $fields = array (
        "mid" => $mid,
        "rid" => $rid,
        "gid" => $gid,
        "tid" => $tid,
        "error" => $errorCode
    );

    db_insert ("atoz_validate_log")
        ->fields ($fields)
        ->execute ();

    $key = atoz_create_key ($mid, $replyTime);

    $result["mid"]   =  $mid;
    $result["gid"]   =  $gid;
    $result["tid"]   =  $tid;
    $result["key"]   = $key;
    $result["rid"]   = $replyTime;
    $result["expiry_date"] = $expiryDate;
    $result["expiry_time"] = $expiryTime;
    $result["error"] = $errorCode;

    $output = json_encode ($result);

    echo $output;
    exit;
}

function atoz_division(){
    $data = array();

    $query = db_select("tbl_home_city", "c");
    $query->fields ("c");

    $result = $query->execute ();

    foreach ($result as $row) {
        $data[$row->cid]    = $row->city_name;
    }

    return $data;
}

function atoz_townships ($division=null) {
    $data = array();
 
    $query = db_select("tbl_home_township", "t");
    $query->fields ("t");
    if($division != null){
        $query->condition ("t.cid", $division);
    }
    $result = $query->execute ();

    foreach ($result as $row) {
        $data[$row->tid]    = $row->township_name;
    }

    return $data;

}
function atoz_get_outlet ($city = null) {
    $data = array();

    $query = db_select("tbl_home_outlets", "t");
    $query->fields ("t");
    if($city)
        $query->condition ("t.cid", $city);

    $result = $query->execute ();

    foreach ($result as $row) {
        $data[$row->goid]    = $row->name;
    }

    return $data;
}

function atoz_consumer_type () {
    return array (
        "2001" => "Domestic",
        "2002" => "Non-Domestic consumption",
    );
}

function atoz_consumer_sub_type ($consumerType=null) {
    if($consumerType=="2002"){
        return array (
            "N1" => "Industries",
            "N2" => "Business",
            "N3" => "Temporary",
            "N4" => "Lamp posts",
            "N5" => "Governmental buildings",
            "N6" => "State-owned industries",
            "N7" => "River Water Pumping Stations",
            "N8" => "Municipal departments,works",
            "N9" => "Non-Governmental organizations",
            "N10" => "Embassies",
            "N11" => "International organizations",
        );
    }else{
        return array (
            "R1" => "Domestic Consumption",
            "R2" => "Power Meter at homes",
            "R3" => "Religious Building",
        );
    }
    
}

function atoz_member_status () {
    return array (
        "1000" => "Pending",
        "1001" => "Active",
    );
}


function atoz_member_types(){
    return array(
        "2000" => "Retail",
        "2001" => "Whole Sale",
    );
}

function atoz_member_level(){
    return array(
        "" => "--Select--",
        "4000" => "Silver",
        "4001" => "Gold",
        "4002" => "Platinum",
	    "4003" => "Diamond"
    );
}


function atoz_get_member_by_id($member_id){

    $data = array();

    $data['name'] = '';
    $data['dob'] = '';
    $data['nrc'] = '';
    $data['phone'] = '';
    $data['current_point'] = 0;
    $data['member_type'] = '';
    $data['city'] = '';
    $data['township'] = '';
    $data['addresss'] = '';
    $data['status'] = '';
    $data['member_level'] = '';
    $data['uid'] = '';
    $data['spent_amount'] = 0;
    $data['point'] = 0;

    $query = db_select('tbl_home_members_info','m');
    $query ->leftjoin('tbl_home_rules','r','m.member_level = r.member_level');
    $query -> fields('m',array('name','dob','nrc','phone','current_point','member_type','city','township','address','status','member_level','uid'));
    $query ->fields('r',array('spent_amount','point'));
    $query -> condition('m.gmid',$member_id);
    $result = $query->execute()->fetchObject();

    if($result){
        foreach ($result as $key => $value) {
            $data[$key] = $value;
        }
    }

    return $data;
}

function atoz_get_user_by_id($uid){
    
    $data = array();

    $data['name'] = '';
    $data['email'] = '';
    $data['phone'] = '';
    $data['department'] = '';
    $data['outlet'] = '';
    $data['city'] = '';
    $data['township'] = '';
    $data['remark'] = '';
    $data['status'] = '';
    $data['guid'] = '';
    $data['rid'] = '';

    $query = db_select('tbl_home_users_info','u');
    $query ->join('users_roles','r','u.uid = r.uid');
    $query -> fields('u',array('name','email','phone','department','outlet','city','township','remark','status','guid'));
    $query -> fields('r',array('rid'));
    $query -> condition('u.uid',$uid);
    $result = $query->execute()->fetchObject();

    if($result){
        foreach ($result as $key => $value) {
            $data[$key] = $value;
        }
    }

    return $data;
}


function getShortUrl ($invoiceID) {
        // check if transaction id exists before getting a url

    $query = db_select ("atoz_meter_invoice", "i");
    $query->leftjoin ("atoz_short_url", "u", "u.transaction_id = i.invoice_id");
    $query->fields ("u", array ("short_url"));
    $query->fields ("i", array ("invoice_id"));
    $query->condition ("i.invoice_id", $invoiceID);
    $result = $query->execute()->fetchAll();

    $shortUrl = "";
    $tid = "";

    foreach ($result as $row) {
        $shortUrl = $row->short_url;
        $tid = $row->invoice_id;
    }

       //if (!$tid) return ""; // transaction doesn't exist

        $now = time ();

        if (!$shortUrl) {
            $retry = 10;
            $inserted = false;

            $candidate = "";

            while ($retry && !$inserted) {
                $prefix = id_base_convert (rand (0, 999));
                $suffix = id_base_convert (time());

                $candidate = $prefix . $suffix;

                $query = db_select ("atoz_short_url", "s");
                $query->condition ("short_url", $candidate);
                $query->addField ("s", "sid");
                $query->range (0, 1);
                $result = $query->execute ();

                $exists = false;

                foreach ($result as $row) {
                    $exists = true;
                    break;
                }

                if (!$exists) {
                    try {
                        $fields = array (
                            "transaction_id"   => $invoiceID,
                            "short_url" => $candidate,
                            "created"   => $now
                        );

                        $query = db_insert ("atoz_short_url");
                        $query->fields ($fields);

                        $result = $query->execute ();

                        if ($result) {
                            $inserted = true;
                        }
                        else {
                            $retry--;
                        }
                    }
                    catch (Exception $e) {
                        error_log ("Error inserting short url" . print_r ($e, true));
                        $retry--;
                    }

                }
                else {
                    $retry--;
                }
            }

            if ($inserted) {
                $shortUrl = $candidate;
            }
        }
        return $shortUrl;
}

function create_invoice_id () {
    $retry = 10;

    while ($retry) {
        $invoiceID = rand(100000,999999) . "-" . time ();

        $query = db_select ("atoz_meter_invoice", "i")
            ->condition("invoice_id", $invoiceID)
            ->fields ("i", array("miid"))
            ->range (0, 1);

        $result = $query->execute();

        $found = false;

        foreach ($result as $row) {
            $found = true;
            break;
        }

        if (!$found) break; // it's unique

        $retry--;

        // no more retries so just add a timestamp to the end to make it unique
        if (!$retry) $invoiceID = $invoiceID;
    }

    return $invoiceID;

}

function atoz_payment_status(){
    return array(0=>'Paid',1=>'Unpaid');
}
function atoz_usage(){
    return array(0=>'1~30',1=>'31~50',2=>'51~75',3=>'76~100');
}
/*function atoz_ward(){
    return array(0=>'Ward1',1=>'Ward2',2=>'Ward3',3=>'Ward4',4=>'Ward5');
}*/

function atoz_ward () {
    return array (
        "0"     =>"Select Ward",
        "3001" => "Ward 1",
        "3002" => "Ward 2",
        "3003" => "Ward 3",
        "3004" => "Ward 4",
        "3005" => "Ward 5",
    );
}


function get_short_url($invoice_id){
    $now = time();
    $qry = db_select ("atoz_meter_invoice", "i");
    $qry->join ("atoz_short_url", "u", "i.invoice_id = u.invoice_id");
    $qry->condition("i.invoice_id",$invoice_id);
    $qry->fields("u");
    $result = $qry->execute()->fetchObject();
    $sms_text = '';
    $shortUrl = '';
    $invoiceID = '';
    if($res){        
        $shortUrl = $res->short_url;
    }else{
        $inserted = false;
        $prefix = id_base_convert(rand (0, 999999));
        $suffix = id_base_convert(time());
        $shortUrl = $prefix.$suffix;
        $invoiceID = $invoice_id;
        $fields = array (
                        "invoice_id"   => $invoiceID,
                        "short_url" => $shortUrl,
                        "created"   => $now
                    );        
        try{
            $query = db_insert("atoz_short_url");
            $query->fields($fields);
            $result = $query->execute();            
        }catch(Exception $exc){
            error_log($exc->errorMessage);
        }        
    }
    $sms_text = $_SERVER["SERVER_NAME"] . "/webapp/print/".$shortUrl;   

    return $sms_text;          
}

function id_base_convert ($inputToConvert, $convertToBaseTen = false) {
//  error_log($inputToConvert);
    $seedDigits = "qQbBzZcCyYdDxXfFwWgGvVhHtTjJsSkKrRlLpPmMnN0123456789";
    $convertedBase = strlen($seedDigits);

    if ($convertToBaseTen) {
        $maxIteration = strlen($inputToConvert)-1;

        $convertedOutput = 0;

        for ($counter=0;$counter<=$maxIteration;$counter++) {
            $currentPower = bcpow($convertedBase,$maxIteration-$counter);
            $digitToFind = substr($inputToConvert,$counter,1);
            $digitPosition = strpos($seedDigits,$digitToFind);
            $valueToAdd = $currentPower*$digitPosition;
            $convertedOutput = $convertedOutput + $valueToAdd;
//          error_log('counter: '.$counter.' inputToConvert: '.$inputToConvert.' currentPower: '.$currentPower.' digitToFind: '.$digitToFind.' digitPosition: '.$digitPosition.' valueToAdd: '.$valueToAdd.' convertedOutput: '.$convertedOutput);
        }
        $convertedOutput = sprintf('%F', $convertedOutput);
        $convertedOutput = substr($convertedOutput, 0, strpos($convertedOutput, '.'));
    }
    else {
        $convertedOutput = '';
        $inputToConvert = $inputToConvert-0;

        for ($counter=floor(log($inputToConvert,$convertedBase));$counter>=0;$counter--) {
            $currentPower = bcpow($convertedBase,$counter);
            $digitValue = floor($inputToConvert/$currentPower)%$convertedBase;
            $convertedOutput = $convertedOutput.substr($seedDigits,$digitValue,1);
//          error_log('counter: '.$counter.' inputToConvert: '.$inputToConvert.' currentPower: '.$currentPower.' digitValue: '.$digitValue.' convertedOutput: '.$convertedOutput);
            $inputToConvert = $inputToConvert - ($digitValue*$currentPower);
        }
    }
//  error_log('theoretically done');
//  error_log($convertedOutput);
    return $convertedOutput;
}

function get_last_invoice($invoiceID=null){
    $query = db_select('atoz_meter_invoice','i');
    $query->fields('i',array('used_amount'));
    $query->condition('i.invoice_id',$invoiceID);
    $data = $query->execute()->fetchObject();   
    return $data;
}
function atoz_generate_key ($appID, $profileID) {
    return md5($appID . "!42" . $profileID . "Win#");
}

function atoz_get_consumer_info_ex ($customerID, $uid = null, $extended = false, $includeInternalData = false, $matchData = true) {

    $data = array();
    //$data["name"]   = "";
    //$data["customerid"]   = "";

    $query = db_select ("atoz_consumer", "c");
    $query->fields ("c");
    $query->condition ("c.consumer_id", $customerID);
    $result = $query->execute()->fetchObject();
    //print_r($result);
    // $data = hti_get_last_transaction ($customerID);
    // if($data){
    //     require_once ("hti_packages.inc");

    //     $packagesAvailable = hti_default_packages ();
    //     $package_amount = 0 ;
    //     $tax_amount = 0;
    //     $finalTotal = 0;
    //     foreach ($packagesAvailable as $desc=>$amount) {
    //         if($desc == $data["package"]){
    //             $package_amount = $amount;
    //             $tax_amount = (5 * $amount) / 100 ;
    //             $finalTotal = $tax_amount + $amount;
    //         }
    //     }

        $data["cid"] = $result->cid;
        $data["meter_id"] = $result->meter_id;
        $data["name"] = $result->name;
        $data["created_by"] = $result->created_by;

         $issue_date = date ("d/M/y");
    //     $lastDate = strtotime ($data["end_date"]);

    //     if ($data["transaction_id"]) {
    //         $lastDate = strtotime ($data["end_date"]);
    //         $billing_start_date = date ("d/M/Y", $lastDate + 24*3600);
    //         $billing_end_date= date("d/M/Y", strtotime("+1 month", $lastDate));
    //         $due_date = date ("d/M/Y", $lastDate + 10*24*3600);
    //     }
    //     else {
    //         $billing_start_date = date ("d/M/Y", time ());
    //         $billing_end_date= date("d/M/Y", strtotime("+1 month", $lastDate));
    //         $due_date = date ("d/M/Y", time () + 10*24*3600);
    //     }

    //     $data["billing_start_date"] = $billing_start_date;
    //     $data["billing_end_date"] = $billing_end_date;
    //     $data["due_date"] = $due_date;

    //     //for plan
    //     $product = $result->product;
    //     $product_plan = '';
    //     if($product == 11000 )
    //         $product_plan = 'FTTH';
    //     else if($product == 12000 )
    //         $product_plan = 'Wifi';
    //     else if($product == 13000 )
    //         $product_plan = 'DIA';
    //     else if($product == 14000 )
    //         $product_plan = 'B-B Small Business Wi-Fi';
    //     else if($product == 15000 )
    //         $product_plan = 'B-B Business Fiber';
    // else if($product == 15001 )
    //         $product_plan = 'FTTH 4G LTE';
    //     else if($product == 999 )
    //         $product_plan = 'Other';

    //     $data["product_plan"] = $product_plan;

    // }
       //  print_r($data);

     return $data;
}


function atoz_admin_pager_by_query ($query, $alias, $field, $dataPerRow, $rowsPerPage, &$currentPage, &$offset, &$total) {

    $currentPage = 1;

    if (isset ($_GET["page"])) {
        $currentPage = $_GET["page"];
    }

    $offset = ($currentPage - 1) * $rowsPerPage;

    $fields =& $query->getFields();
    $expressions =& $query->getExpressions();


    $fields = array ();
    $expressions = array ();

    // add to put something there but don't rely on it because it's wrong if group has a group
    $query->addField ($alias, $field, "placeholder");

    $query->range (0, 1000000000);

    $result = $query->execute();

    if($total > 0 ){
        $total = $total;
    }else{
        $total = $result->rowCount ();
    }

    // echo $query;exit();
    $numOfPages = (int) ($total / $rowsPerPage);

    if ($numOfPages * $rowsPerPage < $total) $numOfPages++;

    $output = "";

    $statusText = '';
    $searchText = '';

    if (isset ($_GET["search"])) {
        $search = $_GET["search"];
        $searchText = "&search=" . $search;
    }

    $prefix = "";

    foreach ($_GET as $parameter=>$value) {
        if ($parameter <> "q" && $parameter <> "page" && $parameter <> "search") {
            if (!$prefix) $prefix .= "?";
            else $prefix .= "&";

            $prefix .= $parameter . '=' . $value;
        }
    }

    if ($numOfPages > 1) {

        if ($prefix) {
            $prefix .= '&';
        }
        else {
            $prefix = '?';
        }

        $output = '
        <div class="mystar-admin-pager">';

        if ($currentPage > 1) {
            $output .= '<a href="'.$prefix.'page=' . ($currentPage - 1) .$searchText. '">&lt;</a> ';
        }

        for ($i = 1; $i <= $numOfPages; $i++) {
            if ($currentPage == $i) {
                $output .= '<span class="ot-selected">'. $i . " </span>";
            }
            else {
                $output .= '<a href="'.$prefix.'page=' . $i . $searchText. '">'. $i.' </a> ';
            }
        }

        if ($currentPage < $numOfPages) {
            $output .= '<a href="'.$prefix.'page=' . ($currentPage + 1) . $searchText. '">&gt;</a> ';
        }

        $output .= '
        </div>';
    }

    

    return $output;
}

function atoz_department(){
    return array(
        '3001' => 'Cashier',
        '3002' => 'HR', 
    );
}

function atoz_get_role(){
    $query = db_select('role','r');
    $query ->fields('r',array('rid','name'));

    $dbOr = db_or();

    $dbOr->condition('r.name','staff admin');
    $dbOr->condition('r.name','super admin');

    $query ->condition($dbOr);

    $result = $query->execute();

    $data = array();

    if($result){
        foreach ($result as $row){
            $data[$row->rid] = $row->name;
        }
    }

    return $data;
}

function myatoz_gifts_by_id($gmid){

    global $user;

    $data = array(''=>'Select Gift'); 

    $query = "select g.grid,g.gift_name,g.gpoint,d.qty from tbl_home_gift g join tbl_home_gift_detail d on g.grid = d.grid where d.qty > 0 and d.qty is not null and d.cid = (select city from tbl_home_users_info where uid = '".$user->uid."') and gpoint <= (select current_point from tbl_home_members_info where gmid = '".$gmid."')";
   
    $result = db_query($query);

    if($result){
        foreach($result as $row){
            $data[$row->grid .'|'.$row->gpoint .'|' .$row->qty] = $row->gift_name;
        }
    }

   return $data;
}

function atoz_get_rule($ruleId){

    $data = array();

    $data['member_level'] = '';
    $data['spent_amount'] = 0;
    $data['point'] = 0;

    $query = db_select('tbl_home_rules','r');
    $query ->fields('r',array('member_level','spent_amount','point'));
    $query ->condition('ruleId',$ruleId);

    $result = $query ->execute()->fetchObject();

    if($result){
        foreach($result as $key=>$value){
            $data[$key] = $value;
        }
    }

    return $data;
}