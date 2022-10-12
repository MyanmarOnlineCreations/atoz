<?php
	function myatoz_promotions($parameter1=null,$parameter2=null,$parameter3=null){

		$output = '';

        if($parameter1 == 'media_detail'){
        	$media_id = $_GET['media_id'];
        	if($media_id == '1'){
        		$contentForm = drupal_get_form ("atoz_notification_form");
        		$output .= drupal_render ($contentForm);
        	}else if($media_id == '2'){
                drupal_goto("atoz/promotions/ads");        		
        	}
        }else if($parameter1 == 'ads'){
            //return $parameter1 . '//'.$parameter2.'//'.$parameter3;
            if ($parameter2 == "edit" && $parameter3) {                
                $contentForm = drupal_get_form("promo_slide_form",$parameter3);
                $output .= drupal_render($contentForm);
            }else if($parameter2 == "new"){
                $contentForm = drupal_get_form("promo_slide_form");
                $output = drupal_render($contentForm);
            }else{
                $output .= myatoz_media_list();
            }
        }else{
        	$output .= myatoz_media_type();
        }
		return $output;
	}
	function myatoz_media_type()
	{
		global $user;
		$output = '<h2 class="list_header">'.t('Choose Media Type').'</h2><br>';
		$output .= '<div class="frm_promo_div">
			    <form method="GET" action="/atoz/promotions/media_detail">
			    	<input type="radio" name="media_id" value="1">
			    	  <label for="html">'.t('Marketing Notification').'</label><br>
			    	<br/>
			    	<input type="radio" name="media_id" value="2">
			    	  <label for="html">'.t('Display Ads').'</label><br>
 				 	<br/>
  					<input class="form-submit" type="submit" value="'.t('Continue').'">
			    </form>
                </div>';
         return $output;
	}
	
function atoz_notification_form ($form, &$form_state) {

    $type = "send_notification";

    $form = array (
     '#id' => 'atoz-'.$type.'-noti'
    );

    $defaultTitle             = "";
    $defaultMessage           = "";

    $access                   = false;
    
    $form["header"] = array (
        '#markup' =>'<h2 class="list_header">' . t('Marketing Notification') . '</h2> <br/>'
    );    

    $form['title'] = array (
        '#title' => t('Message Title'),
        '#type'  => 'textfield',
        '#maxlength' => 255,
        '#default_value' => $defaultTitle
    );

    $form['message'] = array (
        '#title' => t('Description'),
        '#type'  => 'textarea',
        '#default_value' => $defaultMessage
    ); 

    $form['submit'] = array (
        '#type' => 'submit',
        '#value' => t('Publish'),
    );

    return $form;
}


function atoz_notification_form_validate ($form, &$form_state) {

}

function atoz_notification_form_submit ($form, &$form_state) {

    global $user;

    $now = date('Y-m-d h:i:s');

    $data = $form_state['values'];

    try{
        $notification_id = atoz_get_unique_profile_id('N');
    }catch(Exception $e){
        print_r($e->getMessage());
    }

   $fields = array();
   $fields['title'] = $data['title'];
   $fields['message'] = $data['message'];
   $fields['creation_date'] = $now;
   $fields['created_by'] = $user->uid;  

   try {
        db_merge("tbl_home_notifications")
        ->fields($fields)
        ->key(array('notification_id'=>$notification_id))
        ->execute();
       
   } catch (Exception $e) {
       watchdog(
            'notification save error',
            'notification save error @error',
            array('@error' => $e->getMessage()),
            WATCHDOG_NOTICE
        );
   }

   try {

        $query = db_select('tbl_home_authentication','a');
        $query ->isNotNull('a.expo_token');
        $query ->fields('a',array('expo_token'));

       $result = $query->execute();
       
   } catch (Exception $e) {
       watchdog(
            'expo token error',
            'expo token error @error',
            array('@error' => $e->getMessage()),
            WATCHDOG_NOTICE
        );
   }

   // $total_count = $result->rowCount();

   $total_count = 4;

   $offset = 99;

   $start_point = 0;

   while($total_count > 0){

        // watchdog(
        //     'total_count',
        //     'total_count @count',
        //     array('@count' => $total_count),
        //     WATCHDOG_NOTICE
        // );

        $query = db_select('tbl_home_authentication','a');
        $query ->isNotNull('a.expo_token');        
        $query ->fields('a',array('expo_token'));
        $query ->range($start_point,$offset);

        $result = $query->execute();

        // $to = array('ExponentPushToken[u16hncH8UQ1FuBGfkvCYPD]');

        foreach($result as $row){
            $to[] = $row->expo_token;
        }

         watchdog(
            'to array',
            'to array @array',
            array('@array' => json_encode($to)),
            WATCHDOG_NOTICE
        );

        $start_point = $start_point + $offset;

        $total_count = $total_count - $offset;

        $payload = array(
            'to' => $to,
            'title' => $data['title'],
            'sound' => 'default',
            'body' => $data['message'],
        );

        $curl = curl_init();

        curl_setopt_array($curl, array(
          CURLOPT_URL => "https://exp.host/--/api/v2/push/send",
          CURLOPT_RETURNTRANSFER => true,
          CURLOPT_SSL_VERIFYHOST=> false,
          CURLOPT_SSL_VERIFYPEER=> false,
          CURLOPT_ENCODING => "",
          CURLOPT_MAXREDIRS => 10,
          CURLOPT_TIMEOUT => 30,
          CURLOPT_HTTP_VERSION => CURL_HTTP_VERSION_1_1,
          CURLOPT_CUSTOMREQUEST => "POST",
          CURLOPT_POSTFIELDS => json_encode($payload),
          CURLOPT_HTTPHEADER => array(
            "Accept: application/json",
            "Accept-Encoding: gzip, deflate",
            "Content-Type: application/json",
            "cache-control: no-cache",
            "host: exp.host"
          ),
        ));

        $response = curl_exec($curl);

         watchdog(
                'cURL response',
                'cURL response @response',
                array('@response' => $response),
                WATCHDOG_NOTICE
            );
        $err = curl_error($curl);

        curl_close($curl);

        if ($err) {
          watchdog(
                'cURL error',
                'cURL error @error',
                array('@error' => $err),
                WATCHDOG_NOTICE
            );
        }
   }  

   drupal_set_message('Successfully Publish');   

}

function myatoz_media_list()
{
    global $user;

    $rowsPerPage = 100;

    $dataPerRow = 1;

    $offset = atoz_admin_page_offset($rowsPerPage);

    $link = $_SERVER["REQUEST_URI"];
    
    $sortOptions = array (
        "default" =>"created_date",
        "created_date" =>"desc",
        "name" => "desc",
        "promo_id"=>"asc",
        "ads_type" => "asc",  

    );

    $headerFields = array (
        t("Name") => "name",
        t("Type") => "ads_type", 
        t("Created Date")=> "created_date",
        t("Action") => ""
    );

    $styleArray = array (
        "",
        "",
        "",
        "",
        "center"
    );
 
    $data = array();
    $query = db_select('tbl_home_promotion','p');
    $query -> fields('p', array(
        'promo_id',
        'name',
        'ads_type',
        'created_date'));    

    if(isset($_SESSION[ATOZ_PROMO_FILTER_TYPE]) && $_SESSION[ATOZ_PROMO_FILTER_TYPE] != '0'){
        $query -> condition('p.ads_type',$_SESSION[ATOZ_PROMO_FILTER_TYPE]);
    }

    $query->range($offset, $rowsPerPage);

    atoz_checksort ($sortOptions, $query);
  
    $result = $query->execute()->fetchAll();
   
    $type_arr = atoz_ads_type();

    foreach($result as $row){
        foreach($row as $key => $value){
            if($key == 'promo_id'){
                continue;
            }else if($key == 'ads_type'){
                $nvalue = $type_arr[$value];
            }else{
                $nvalue = $value;
            }
            $list[$key] = $nvalue;
        }
        $list['action'] = '<a href="/atoz/promotions/ads/edit/'.$row->promo_id.'">Edit</a>';
        $data[] = $list;
    }
    $output .= '<a href="/atoz/promotions/ads/new"> + '.t('New Promotions').'</a>';
    $header = atoz_generateHeader ($headerFields, $sortOptions);
    
    $pager = atoz_admin_pager_by_query($query,"p", "id", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);
    
    $output .= '<h2 class="list_header">'.t('Promotion').'<span class="atoz-admin-title-total">('.$total.')</span></h2>';

    $output .=  $pager . atoz_admin_create_table ("myst-admin-table", $header, $data, $styleArray) . $pager;

    return $output;
}

function promo_slide_form ($form, &$form_state, $promoID = null) {
    $type="promo_slide";
    $form = array (
     '#id' => 'atoz-promo_slide-edit'
    );

    $defaultName              = "";
    $defaultNamemm            = "";
    $defaultDescription       = "";
    $defaultDescriptionMM     = "";
    $defaultType              = "";
    $defaultImageEN           = "";
    $defaultImageMM           = "";
    $defaultCreationDate      = "";
    $defaultModifiedDate      = "";

    $access                   = false;


    
      // if we're editing an existing location
    if ($promoID) {
        $access = true;

        $found = false;

        $result = atoz_get_promo_lists($promoID);
        //print_r($result);exit();
        foreach ($result as $row) {
            $defaultName              = $row->name;
            $defaultNamemm            = $row->namemm;
            $defaultDescription       = $row->description;
            $defaultDescriptionMM     = $row->descriptionmm;
            $defaultType              = $row->ads_type;
            $defaultImageEN           = $row->image_en;
            $defaultImageMM           = $row->image_mm;
            $defaultCreationDate      = $row->created_date;
            $defaultModifiedDate      = $row->modified_date;

            $found = true;
        }

        if (!$found) {
            $form["header"] = array (
                '#markup' =>
        '<div class="atoz-form-header">' . t('Cannot edit this Promotion.') . '</div>'
            );

            return $form;
        }
    }

    if (!$promoID) {
        $form["header"] = array (
                '#markup' =>
        '<div class="atoz-form-header">' . t('Add New Promo Slide') . '</div>'
            );
    }

    $form['name'] = array (
        '#title' => t('Name (English)'),
        '#type'  => 'textfield',
        '#maxlength' => 255,
        '#default_value' => $defaultName,
        '#required' => true,
    );

    $form['namemm'] = array (
        '#title' => t('Name (Myanmar)'),
        '#type'  => 'textfield',
        '#maxlength' => 255,
        '#default_value' => $defaultNamemm,
    );
    
    $form['description'] = array (
        '#title' => t('Description (English)'),
        '#type'  => 'textarea',
        '#default_value' => $defaultDescription
    );

    $form['descriptionmm'] = array (
        '#title' => t('Description (Myanmar)'),
        '#type'  => 'textarea',
        '#default_value' => $defaultDescriptionMM
    );
    
     $promotype = atoz_ads_type();
    $form["ads_type"] = array (
        '#title' => t('Type'),
        '#type' => 'select',
        '#options' => $promotype,
        '#default_value' => $defaultType,
        '#required' => true,
    );
    //For image
    $imageUrl="";

    if ($defaultImageEN) {
        $imageUrl = '
                <img  src="' . atoz_web_path ($type) .  $defaultImageEN . '" />
            ';
    }
    else {
        $imageUrl = '
                <img  src="/sites/all/modules/atoz/images/no_image.png" />
            ';
    }

    $form['image_block'] = array (
        '#type' => 'fieldset',
        '#title' => t('Image'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#tree' => false,
        '#group' => 'image_group'
    );

    $form['image_block']['image'] = array(
        '#title' => '
        <div id="image-title">'. t('Tap image to change') . '</div>
        <div id="container">
            <div id="pick-image">' . $imageUrl . '
            </div>
        </div>',
        '#type' => 'plupload',
        '#upload_validators' => array(
            'file_validate_extensions' => array('png jpg jpeg'),
        ),
        '#autoupload' => TRUE,
        '#submit_element' => '#edit-submit',
        '#plupload_settings' => array(
            'runtimes' => 'html5',
            // note we don't preserver headers i.e. exif so it will rotate as required on client side
            'resize' => array ("width"=>"1024", "height"=>"1024", "quality"=>"85", "crop"=>false, "preserve_headers"=> false),
            'max_file_size' => 1024*1024*20 . 'b', // file_upload_max_size() . 'b',
            'unique_names' => TRUE,
            'browse_button' => 'pick-image',
            'container' => 'container',
        ),
        '#event_callbacks' => array (
            "UploadComplete" => "upload_completed",
            "UploadFile" => "upload_file",
        ),
    );

    $removable = false;

    if ($defaultImageEN) $removable = true;

    $form['image_block']['remove_image'] = array (
        '#title'  => 'Remove image',
        '#type'  => 'checkbox',
        '#default_value' => 0,
        '#access' => $removable
    );

    // //image for myanmar
    $imagemm_Url="";

    if ($defaultImageMM) {
        $imagemm_Url = '
                <img  src="' . atoz_web_path ($type) .  $defaultImageMM . '" />
            ';
    }
    else {
        $imagemm_Url = '
                <img  src="/sites/all/modules/atoz/images/no_image.png" />
            ';
    }

    $form['imagemm_block'] = array (
        '#type' => 'fieldset',
        '#title' => t('Image_MM'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#tree' => false,
        '#group' => 'imagemm_group'
    );

    $form['imagemm_block']['imagemm'] = array(
        '#title' => '
        <div id="imagemm-title">'. t('Tap image to change') . '</div>
        <div id="containermm">
            <div id="pick-imagemm">' . $imagemm_Url . '
            </div>
        </div>',
        '#type' => 'plupload',
        '#upload_validators' => array(
            'file_validate_extensions' => array('png jpg jpeg'),
        ),
        '#autoupload' => TRUE,
        '#submit_element' => '#edit-submit',
        '#plupload_settings' => array(
            'runtimes' => 'html5',
            // note we don't preserver headers i.e. exif so it will rotate as required on client side
            'resize' => array ("width"=>"1024", "height"=>"1024", "quality"=>"85", "crop"=>false, "preserve_headers"=> false),
            'max_file_size' => 1024*1024*20 . 'b', // file_upload_max_size() . 'b',
            'unique_names' => TRUE,
            'browse_button' => 'pick-imagemm',
            'container' => 'containermm',
        ),
        '#event_callbacks' => array (
            "UploadComplete" => "upload_completedmm",
            "UploadFile" => "upload_filemm",
        ),
    );

    $removablemm = false;

    if ($defaultImageMM) $removablemm = true;

    $form['imagemm_block']['remove_imagemm'] = array (
        '#title'  => 'Remove image MM',
        '#type'  => 'checkbox',
        '#default_value' => 0,
        '#access' => $removablemm
    );

    $form['submit'] = array (
        '#type' => 'submit',
        '#value' => t('Publish'),
    );

    $form['confirm_delete'] = array (
        '#title' => t('Confirm delete (check this if you are going to delete this series.)'),
        '#type'  => 'checkbox',
        '#weight' => 98,
        '#default_value' => "",
        '#access' => $access,
        '#prefix' => '<hr />'
    );

    $form['delete'] = array (
        '#type' => 'submit',
        '#weight' => 99,
        '#access' => $access,
        '#value' => t('Delete')
    );

    $form['promoid'] = array (
        '#type' => 'hidden',
        '#value' => $promoID
    );

    $form['promo_image'] = array (
        '#type' => 'hidden',
        '#value' => $defaultImageEN
    );
    $form['promo_imagemm'] = array (
        '#type' => 'hidden',
        '#value' => $defaultImageMM
    );

    return $form;
}


function promo_slide_form_validate ($form, &$form_state) {

    $data = $form_state["values"];

    $promoID = $data["promoid"];

    if ($promoID) {
        if ($data["op"] ==  $data["delete"] ) {
            if (!$data["confirm_delete"]) {
                form_set_error ("confirm_delete", t('To delete a series, you must check the "Confirm delete" checkbox and click Delete'));
            }

        }
    }

}

function promo_slide_form_submit ($form, &$form_state) {

    // load up the values so it's easier to save

    $data = $form_state["values"];

    $promoID = $data["promoid"];
    
    $exists = true;

    if (!$promoID) {
        $promoID = atoz_unique_promo_id ();
        $exists = false;
    }

    global $user;

    $now = date('Y-m-d h:i:s');

    $imageName = $data["promo_image"];
    $fileUploaded = false;

    // check if we uploaded a file
    foreach ($data['image'] as $uploaded_file) {
        if ($uploaded_file['status'] == 'done') {
            $fileSource = $uploaded_file['tmppath'];
            //echo $fileSource;exit();
            $extension = strtolower (pathinfo ($uploaded_file['name'], PATHINFO_EXTENSION));

            $newImageName = str_replace (".", "_", $promoID."_".time()). "." . $extension;

            if ($imageName <> $newImageName) {
                atoz_delete_file ($imageName, "promo_slide");
                $imageName = $newImageName;
            }

            atoz_save_file ($fileSource, $imageName, FILE_EXISTS_REPLACE, "promo_slide");

            $fileUploaded = true;
            break;
        }
        else {
            // @todo: move this to element validate or something and clean up t().
            // form_set_error('pud', "Upload of {$uploaded_file['name']} failed");
        }
    }
    if ($imageName && !$fileUploaded && $data["remove_image"]) {
        atoz_delete_file ($imageName, "promo_slide");
        $imageName = "";
    }

    $imagemm_Name = $data["promo_imagemm"];
    $filemm_Uploaded = false;

    // check if we uploaded a file
    foreach ($data['imagemm'] as $uploaded_file) {
        if ($uploaded_file['status'] == 'done') {
            $fileSource = $uploaded_file['tmppath'];

            $extension = strtolower (pathinfo ($uploaded_file['name'], PATHINFO_EXTENSION));

            $newImageMMName = str_replace (".", "_", $slideID."_".time()."_mm"). "." . $extension;


            if ($imagemm_Name <> $newImageMMName) {
                atoz_delete_file ($imagemm_Name, "promo_slide");
                $imagemm_Name = $newImageMMName;
            }

            atoz_save_file ($fileSource, $imagemm_Name, FILE_EXISTS_REPLACE, "promo_slide");

            $filemm_Uploaded = true;
            break;
        }
        else {
            // @todo: move this to element validate or something and clean up t().
            // form_set_error('pud', "Upload of {$uploaded_file['name']} failed");
        }
    }
    if ($imagemm_Name && !$filemm_Uploaded && $data["remove_imagemm"]) {
        atoz_delete_file ($imagemm_Name, "promo_slide");
        $imagemm_Name = "";
    }



    if ($exists) {
        if ($data["op"] == $data["delete"]) {

            db_delete ("tbl_home_promotion")
                ->condition ("promo_id", $promoID)
                ->execute ();

            if ($imageName) atoz_delete_file ($imageName, "promo_slide");
            if ($imagemm_Name) atoz_delete_file ($imagemm_Name, "promo_slide");
            drupal_set_message (t('Promotion has been deleted.'));
        }
        else {

            $fields = array (
                "name"             => $data["name"],
                "namemm"           => $data["namemm"],
                "description"      => $data["description"],
                "descriptionmm"    => $data["descriptionmm"],
                "ads_type"         => $data["ads_type"],
                "image_en"         => $imageName,
                "image_mm"         => $imagemm_Name,
                "modified_date"    => $now
            );

            $result = db_merge ("tbl_home_promotion")
                ->key (array ("promo_id"=> $promoID))
                ->fields ($fields)
                ->execute ();
            if($result){
                drupal_set_message (t('Promotion has been updated.'));
            }else{
                drupal_set_message (t('Add Promotion failed, please contact your system administrator.'));
            }
        }
    }
    else {
        $fields = array (
                "promo_id"         => $promoID,
                "name"             => $data["name"],
                "namemm"           => $data["namemm"],
                "description"      => $data["description"],
                "descriptionmm"    => $data["descriptionmm"],
                "ads_type"         => $data["ads_type"],
                "image_en"         => $imageName,
                "image_mm"         => $imagemm_Name,
                "created_date"     => $now,
                "modified_date"    => $now
            );
        $result = db_insert ("tbl_home_promotion")
            ->fields ($fields)
            ->execute ();

        if ($result) {
            drupal_set_message (t('Promotion has been added.'));
        }
        else {
            drupal_set_message (t('Add promotion failed, please contact your system administrator.'));
        }
    }

    drupal_goto ("/atoz/promotions/ads");
}
function atoz_unique_promo_id () {
    $retry = 10;

    $uniqueID = "";

    while ($retry) {
        $uniqueID = uniqid ("PR", true);

        $query = db_select ("tbl_home_promotion", "p")
            ->condition("promo_id", $uniqueID)
            ->fields ("p", array("promo_id"))
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
?>