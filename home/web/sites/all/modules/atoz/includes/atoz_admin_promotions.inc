<?php
	function myatoz_promotions($parameter1=null,$parameter2=null){
		$output = '';

        if($parameter1 == 'media_detail'){
        	$media_id = $_POST['media_id'];
        	if($media_id == '1'){
        		$contentForm = drupal_get_form ("send_notification_form", $parameter2);
        		$output .= drupal_render ($contentForm);
        	}else if($media_id == '2'){
        		$output .= myatoz_display_ads();
        	}
        }else{
        	$output .= myatoz_media_type();
        }
		return $output;
	}
	function myatoz_media_type()
	{
		global $user;
		$output = '<h2 class="list_header">Choose Media Type</h2><br>';
		$output .= '<div class="frm_promo_div">
			    <form method="POST" action="/atoz/promotions/media_detail">
			    	<input type="radio" name="media_id" value="1">
			    	  <label for="html">Marketing Notifications</label><br>
			    	<br/>
			    	<input type="radio" name="media_id" value="2">
			    	  <label for="html">Display Ads</label><br>
 				 	<br/>
  					<input class="form-submit" type="submit" value="Submit">
			    </form>
                </div>';
         return $output;
	}
	
	function send_notification_form ($form, &$form_state) {

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


function send_notification_form_validate ($form, &$form_state) {


}
function send_notification_form_submit ($form, &$form_state) {

    // load up the values so it's easier to save

    $data = $form_state["values"];

    $notiID = $data["notiid"];

    $exists = true;

    if (!$notiID) {
        $notiID = hti_unique_notificaion_id();
        $exists = false;
    }

    global $user;

    $now = gmdate('Y-m-d G:i:s');


    if ($exists) {
        if ($data["op"] == $data["delete"]) {

            db_delete ("hti_notification")
                ->condition ("notification_id", $notiID)
                ->execute ();



            drupal_set_message (t('Notification has been deleted.'));
        }
        else {

            $fields = array (
                "title"            => $data["title"],
                "message"          => $data["message"],
                "status"           => "1",
                "type"             => $data["type"],
                "modified_date"    => $now
            );

            db_merge ("hti_notification")
                ->key (array ("notification_id"=> $notiID))
                ->fields ($fields)
                ->execute ();
            drupal_set_message (t('Notification has been updated.'));
        }
    }
    else {

        $fields = array (
                "notification_id"  => $notiID,
                "title"            => $data["title"],
                "message"          => $data["message"],
                "status"           => "1",
                "type"             => $data["type"],
                "creation_date"    => $now,
                "modified_date"    => $now
            );
        $result = db_insert ("hti_notification")
            ->fields ($fields)
            ->execute ();

        if ($result) {
            drupal_set_message (t('Notification has been added.'));
        }
        else {
            drupal_set_message (t('Add Notification failed, please contact your system administrator.'));
        }
    }

    drupal_goto ("hti/send_notification");
}

function myatoz_display_ads()
{
	return "myatoz_display_ads";
}
?>