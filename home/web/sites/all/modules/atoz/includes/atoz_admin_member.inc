<?php

	date_default_timezone_set("Asia/Yangon");

	function myatoz_member($parameter1=null,$parameter2=null){
		$output = '';
		if($parameter1 == 'new'){
			$contentForm = drupal_get_form('myatoz_member_form',$parameter2='');
			$output .= drupal_render($contentForm);
		}else if($parameter1 == 'edit' && $parameter2){
			$contentForm = drupal_get_form('myatoz_member_form',$parameter2);
			$output .= drupal_render($contentForm);
		}else if($parameter1 == 'point_collect'){
			$output .= myatoz_member_point_collect($parameter2);
		}else if($parameter1 == 'point_redeem'){
			$output .= myatoz_member_point_redeem($parameter2);
		}else{
			$contentForm = drupal_get_form('myatoz_member_filter_form');
			$output .= drupal_render($contentForm);
			$output .= myatoz_member_list();
		}
		return $output;
	}

	function myatoz_member_list(){

		global $user;

		$output="";
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
		$offset = atoz_admin_page_offset($rowsPerPage);		
		
    	$data = array();

		$sortOptions = array (
			"default"			=> "name",
			"name"				=> "desc",
			"gmid"				=> "desc",
			"current_point"     => "desc",
			"lp_amount"			=> "desc",
			"city"				=> "desc",
			"township"			=> "township",
			"status"			=> "desc",	
			"created_by"		=> "desc",
			"created_date"		=> "desc",
		);

		$styleArray = array (
			"",
			"",
			"",
			"right",
			"",
			"",
			"",
			"",
			"",
			"",
		);

	   $headerFields = array (
			t("Customer Name")      => "name",
        	t("Member ID") 			=> "gmid",
        	t("Point Collect")		=> "current_point",
        	t("Last Purchase Amount")=> "lp_amount",
        	t("State & Region") 	=> "city",
        	t("Township")			=> "township",
        	t("Status")				=> "status",
        	t("Created By")			=> "created_by",
        	t("Creation Date")		=> "created_date",
        	t("Remark")				=> "",	
	   );

	   if(in_array('super admin', $user->roles) || in_array('staff admin', $user->roles)){
	   		$headerFields[t('Action')] = '';
	   }
		$query = db_select('tbl_home_members_info','m');
		$query ->leftjoin('users','u','m.created_by = u.uid');
		$query -> fields('m');
		$query -> addField('u','name','created_name');

		$query->range($offset, $rowsPerPage);

	    atoz_checksort ($sortOptions, $query);

		if(isset($_SESSION[ATOZ_USER_ID_FILTER])){
			$query->condition('gmid',$_SESSION[ATOZ_USER_ID_FILTER]);
		}

		if(isset($_SESSION[ATOZ_MEMBER_CITY_FILTER])){
			$query->condition('city',$_SESSION[ATOZ_MEMBER_CITY_FILTER]);
		}

		if(isset($_SESSION[ATOZ_MEMBER_TOWNSHIP_FILTER])){
			$query->condition('township',$_SESSION[ATOZ_MEMBER_TOWNSHIP_FILTER]);
		}

		if(isset($_SESSION[ATOZ_MEMBER_TYPE_FILTER])){
			$query->condition('member_type',$_SESSION[ATOZ_MEMBER_TYPE_FILTER]);
		}

		$result = $query->execute();

		$status_arr = atoz_member_status();

		$city_arr = atoz_division();

		$township_arr = atoz_townships();		

		if($result){

			foreach($result as $row){

				if($row->created_name == null){
					$created_name = 'Anonymous';
				}else{
					$created_name = $row->created_name;
				}

				$list = array();					
				$list['name'] = $row->name;
				$list['gmid'] = $row->gmid;
				$list['current_point'] = number_format($row->current_point);	
				$list['lp_amount']	 = number_format($row->lp_amount);	
				$list['city']	= $city_arr[$row->city];
				$list['township'] = $township_arr[$row->township];	
				$list['status'] = $status_arr[$row->status];
				$list['created_by'] = $created_name;
				$list['created_date'] = date('Y-m-d',strtotime($row->created_date));

				if($row->status == '1001'){
					$list['remark'] = '<a href="/atoz/member/point_collect/'.$row->gmid.'">Collect Points</a> / <a href="/atoz/member/point_redeem/'.$row->gmid.'">Redeem Point</a>';
				}else{
					$list['remark'] = '';
				}

				if(in_array('super admin',$user->roles) || in_array('staff admin' , $user->roles)){
					$list['action'] = '<a href="/atoz/member/edit/'.$row->gmid.'">Edit</a>';
				}

				$data[] = $list;				
			}			
		}


		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$output .= '<a href="/atoz/member/new"> + '.t('New Member').'</a>';


		$pager = atoz_admin_pager_by_query($pageQuery,"m", "gmid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header"> '.t('Members').' <span class="myst-admin-title-total">('.$total.')</span></h2>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;
	}

	function myatoz_member_filter_form($form, &$form_state){

		if (isset ($_SESSION[ATOZ_USER_ID_FILTER])) {
	        $defaultUserIDFilter = $_SESSION[ATOZ_USER_ID_FILTER];
	    }
	    else {
	        $defaultUserIDFilter = "";
	    } 

	    if(isset($_SESSION[ATOZ_MEMBER_CITY_FILTER])){
	    	$defaultCityFilter = $_SESSION[ATOZ_MEMBER_CITY_FILTER];
	    }else{
	    	$defaultCityFilter = "";
	    }

	    if(isset($_SESSION[ATOZ_MEMBER_TOWNSHIP_FILTER])){
	    	$defaultTownshipFilter = $_SESSION[ATOZ_MEMBER_TOWNSHIP_FILTER];
	    }else{
	    	$defaultTownshipFilter = "";
	    }

	    if(isset($_SESSION[ATOZ_MEMBER_TYPE_FILTER])){
	    	$defaultMemberType = $_SESSION[ATOZ_MEMBER_TYPE_FILTER];
	    }else{
	    	$defaultMemberType = "";
	    }

		$form = array('#id' => 'my-atoz-user-filter-form');	

		$form['user_id'] = array (
		    '#title' => t('User ID'),
	        '#type' => 'textfield',
	        '#default_value' => $defaultUserIDFilter,
	    );

	    $city_arr = atoz_division();
	    $city_arr[""] = t("-- Select --");

	    $form['city'] = array(
	    	'#title' => t('State & Region'),
	    	'#type'  => 'select',
	    	'#options' => $city_arr,
	    	'#default_value' => $defaultCityFilter,
	    );

	    $township_arr = atoz_townships();
	    $township_arr[""] = t("-- Select --");	    

	    $form['township'] = array(
	    	'#title' => t('Township'),
	    	'#type'  => 'select',
	    	'#options' => $township_arr,
	    	'#default_value' => $defaultTownshipFilter,
	    );

	    $member_type_arr = atoz_member_types();

	    $form['member_type'] = array(
	    	'#title' => t('Member Type'),
	    	'#type'  => 'select',
	    	'#options' => $member_type_arr,
	    	'#default_value' => $defaultMemberType,
	    );

	    $form['submit'] = array (
	        '#type' => 'submit',
	        '#weight' => 98,
	        '#value' => t('Search')
	    );

	    $form['reset'] = array (
	        '#type' => 'submit',
	        '#weight' => 99,
	        '#value' => t('Reset')
	    );

		return $form;
	}

	function myatoz_member_filter_form_validate($form, &$form_state){

	}

	function myatoz_member_filter_form_submit($form, &$form_state){

		if ($form_state["values"]["op"] == t("Reset")) {

			unset ($_SESSION[ATOZ_USER_ID_FILTER]);
			unset ($_SESSION[ATOZ_MEMBER_CITY_FILTER]);
			unset ($_SESSION[ATOZ_MEMBER_TOWNSHIP_FILTER]);
			unset ($_SESSION[ATOZ_MEMBER_TYPE_FILTER]);
			
		}else{
			
			$userIdFilter = $form_state["values"]["user_id"];
			
			if (!$userIdFilter) {
				unset ($_SESSION[ATOZ_USER_ID_FILTER]);
			}
			else {
				$_SESSION[ATOZ_USER_ID_FILTER] = $userIdFilter;
			}

			$cityFilter = $form_state["values"]["city"];
			
			if (!$cityFilter) {
				unset ($_SESSION[ATOZ_MEMBER_CITY_FILTER]);
			}
			else {
				$_SESSION[ATOZ_MEMBER_CITY_FILTER] = $cityFilter;
			}

			$townshipFilter = $form_state["values"]["township"];
			
			if (!$townshipFilter) {
				unset ($_SESSION[ATOZ_MEMBER_TOWNSHIP_FILTER]);
			}
			else {
				$_SESSION[ATOZ_MEMBER_TOWNSHIP_FILTER] = $townshipFilter;
			}
			
			$memberTypeFilter = $form_state["values"]["member_type"];

			if (!$memberTypeFilter) {
				unset ($_SESSION[ATOZ_MEMBER_TYPE_FILTER]);
			}
			else {
				$_SESSION[ATOZ_MEMBER_TYPE_FILTER] = $memberTypeFilter;
			}	
			
		}

	}
	
	function myatoz_member_form($form, &$form_state, $member_id = ''){

		$data = array();

		$data = atoz_get_member_by_id($member_id);

		if($member_id == ''){
			$form_title = 'New Member';
			$submit_title = 'Create';
		}else{
			$form_title = 'Edit Member';
			$submit_title = 'Update';
		}

		$form = array('#id' => 'my-atoz-user-edit');

		$form['title'] = array (
	      	'#markup' => t($form_title),
	    );		

		$form['name'] = array (
		    '#title' => t('Name'),
	        '#type' => 'textfield',
	        '#default_value' => $data["name"],
	        '#required' =>true,

	    );

	    $form["dob"] = array (
			'#title' => t('Date Of Birth'),
			'#type' => 'date_popup',
			'#default_value' => $data["dob"],
			'#date_format' => 'Y-m-d',
			'#date_year_range' => '-100:+1',
	        '#required' =>true,

		);

	    $form['nrc'] = array (
		    '#title' => t('NRC Number'),
	        '#type' => 'textfield',
	        '#default_value' => $data["nrc"],
	    );

	    $form['phone'] = array (
		    '#title' => t('Phone Number'),
	        '#type' => 'textfield',
	        '#default_value' => $data["phone"],
	        '#required' =>true,
	    );

	    $gender_arr = atoz_gender();

	    $form['gender'] = array (
		    '#title' => t('Gender'),
	        '#type' => 'select',
	        '#options' => $gender_arr,
	        '#default_value' => $data["gender"],
	        '#required' =>true,


	    );

	    $form['current_point'] = array (
		    '#title' => t('Current Point'),
	        '#type' => 'textfield',
	        '#default_value' => $data["current_point"],
	        '#required' =>true,

	    );

	    $member_type_arr = atoz_member_types();

	    $form['member_type'] = array (
		    '#title' => t('Membership Type'),
	        '#type' => 'select',
	        '#options' => $member_type_arr,
	        '#default_value' => $data["member_type"],
	        '#required' =>true,
	    );

	    $city_arr = atoz_division();

	    $form['city'] = array (
		    '#title' => t('State & Region'),
	        '#type' => 'select',
	        '#options'=>$city_arr,
	        '#default_value' => $data["city"],
	        '#required' =>true,

	    );
	    
	    for($i = 1; $i <= 15 ; $i++){

	    	$township_arr = atoz_townships($i);

		    $form['township'.$i] = array (
			    '#title' => t('Township'),
		        '#type' => 'select',
		        '#options'=>$township_arr,
		        '#default_value' => $data["township"],
	        	'#required' =>true,
		    );
	    }

	    $form['address'] = array (
		    '#title' => t('Address'),
	        '#type' => 'textarea',
	        '#default_value' => $data["address"],       
	    );

	    if($member_id == ''){
	    	$form['password'] = array (
			    '#title' => t('Password'),
		        '#type' => 'password',
		        '#required' => true,
		    );

		    $form['confirm_password'] = array (
			    '#title' => t('Confirm Password'),
		        '#type' => 'password',
		        '#required' => true,
		    );
	    }

	    if($member_id != ''){
	    	$status_arr = atoz_member_status();
	    	$form['status'] = array(
	    		'#title' => t('Status'),
		        '#type' => 'select',
		        '#options'=>$status_arr,
		        '#default_value' => $data["status"],
	    	);

	    	$member_level_arr = atoz_member_level();
	    	$form['member_level'] = array(
	    		'#title' => t('Member Tier'),
		        '#type' => 'select',
		        '#options'=>$member_level_arr,
		        '#default_value' => $data["member_level"],
	    	);
	    }

	    $form['gmid'] = array(
	    	'#type' => 'hidden',
	    	'#default_value' => $member_id,
	    );

	    $form['uid'] = array(
	    	'#type' => 'hidden',
	    	'#default_value' => $data['uid'],
	    );


	    $form['save'] = array(
		    '#type' => 'submit',
		    '#value' => t($submit_title),
		);

		return $form;
	}

	function myatoz_member_form_validate($form, &$form_state){
		if ($form_state['values']['password'] !== $form_state['values']['confirm_password']) { 

			form_set_error('password', t('Password does not match'));

			form_set_error('confirm_password', t('Password does not match'));
		}
	}

	function myatoz_member_form_submit($form, &$form_state){

		global $user;

		$now = date('Y-m-d h:i:s');

		$data = $form_state["values"];

		$fields = array();

		$uid = $data['uid'];


		$fields = array(
			
			"name" => $data['name'],
			"dob" => $data['dob'],
			"nrc" => $data['nrc'],
			"phone" => $data['phone'],
			"current_point" => $data['current_point'],
			"member_type" => $data['member_type'], 
			"township" => $data['township'.$data['city']],
			"city" => $data['city'],
			"address" => $data['address'],
			"gender" => $data['gender'],
		);

		if($data["gmid"] == ''){

			$fields['created_date'] = $now;
			$fields['created_by'] = $user->uid;

			$gmid = atoz_get_unique_member_id($data['member_type']);

			$account = new stdClass();

		    $account->is_new = true;
		    // temporary name
		    $username = rand(1000, 9999) . $now;    

		    $details["mail"] = $username;
		    $details["name"] = $username;
		    $details["status"] = 1; //default status is active;
		    $details["pass"] = $data['password'];

		    $account = user_save($account,$details);
		    $uid = $account->uid;

		    if ($uid) {
		        // creating staff account
		        $username = $gmid;
		        $email = $data['email'];
		        $details2["mail"] = $email;
		        $details2["name"] = $username;
		        $account = user_save($account,$details2);
		    }

		    $fields['uid'] = $uid;		    

		}else{
			$gmid = $data["gmid"];
			$fields['updated_by'] = $user->uid;
			$fields['updated_date'] = $now;
		}		

		if(isset($data['status'])){
			$fields['status'] = $data['status'];
		}

		if(isset($data['member_level'])){
			$fields['member_level'] = $data['member_level'];	
		}

		try{
			db_merge ("tbl_home_members_info")
        	->key(array("gmid"=>$gmid))
            ->fields ($fields)
            ->execute();
			
        	drupal_set_message('Successfully Save');

		}catch(Exception $e){
        	 drupal_set_message($e->getMessage(), 'Err');
		}

		drupal_goto('/atoz/member');   		

	}

	function myatoz_member_point_collect($parameter2){

		$data = array();

		$output = '';

		$data = atoz_get_member_by_id($parameter2);

		$member_level_arr = atoz_member_level();

		$output .= 
		'<h2>'.t('Collect Points').'</h2>
		<table>
			<tr>
				<td>'.t('Name').'</td>
				<td>'.$data["name"].'</td>
			</tr>
			<tr>
				<td>'.t('Member ID').'</td>
				<td>'.$parameter2.'</td>
			</tr>
			<tr>
				<td>'.t('NRC No.').'</td>
				<td>'.$data["nrc"].'</td></tr>
			<tr>
				<td>'.t('Date Of Birth').'</td>
				<td>'.$data["dob"].'</td>
			</tr>
			<tr>
				<td>'.t('Phone Number').'</td>
				<td>'.$data["phone"].'</td>
			</tr>
			<tr>
				<td>'.t('Total Points Collected').'</td>
				<td>'.$data["current_point"].'</td>
			</tr>	
			<tr>
				<td>'.t('Member Level').'</td>
				<td>'.$member_level_arr[$data["member_level"]].'</td>
			</tr>			
		</table>';

		$contentForm = drupal_get_form('atoz_point_collect_form',$parameter2,$data['member_level'],$data["spent_amount"],$data["point"]);

		$output .= drupal_render($contentForm);

		return $output;

	}

	function atoz_point_collect_form($form, &$form_state,$gmid,$member_level,$spent_amount,$point){

		$form = array('#id' => 'my-atoz-point-collect-edit');

		$form['title'] = array (
	      	'#markup' => '',
	    );		

		$form['invoice_id'] = array (
		    '#title' => t('Invoice No'),
	        '#type' => 'textfield',
	        '#required' =>true,
	    );

	    $form["total_amount"] = array (
			'#title' => t('Total Amount'),
			'#type' => 'textfield',
	        '#required' =>true,
		);

	    $form['point'] = array (
		    '#title' => t('Point'),
	        '#type' => 'textfield',
	        '#required' =>true,
	    );

	    $form['gmid'] = array (		    
	        '#type' => 'hidden',
	        '#default_value' =>$gmid,
	    );

	    $form['member_level'] = array (		    
	        '#type' => 'hidden',
	        '#default_value' =>$member_level,
	        '#attributes' => array('id'=>'edit-member-level'),
	    );

	    $form['spent_amount'] = array (		    
	        '#type' => 'hidden',
	        '#default_value' =>$spent_amount,
	        '#attributes' => array('id'=>'edit-spent-amount'),
	    );

	    $form['get_point'] = array (		    
	        '#type' => 'hidden',
	        '#default_value' =>$point,
	        '#attributes' => array('id'=>'edit-get-point'),
	    );

	    $form['save'] = array(
		    '#type' => 'submit',
		    '#value' => t('Collect Now'),
		);

	    return $form;
	}

	function atoz_point_collect_form_validate($form, &$form_state){

	}

	function atoz_point_collect_form_submit($form, &$form_state){

		global $user;

		$now = date('Y-m-d G:i:s');

		$data = $form_state['values'];

		$fields = array();

		$current_point = 0;

		$gmid = $data['gmid'];

		$query = db_select('tbl_home_members_info','m');
		$query ->condition('m.gmid', $gmid);
		$query ->fields('m',array('current_point'));
		$result = $query -> execute()->fetchObject();

		if($result){
			$current_point = $result->current_point;
		}

		$fields['point'] = $data['point'];
		$fields['invoice_id'] = $data['invoice_id'];
		$fields['amount'] = $data['total_amount'];
		$fields['created_date'] = $now;
		$fields['created_by'] = $user->uid;
		$fields['updated_date'] = $now;
		$fields['updated_by'] = $user->uid;

		if($fields['point'] > 0 ){

			$current_point = $current_point + $fields['point'];

			$mfields = array(
				'current_point'=>$current_point,
				'lp_amount' => $data['total_amount'],
			);

			db_merge ("tbl_home_members_info")
        	->key(array('gmid'=>$gmid))
            ->fields ($mfields)
            ->execute();
		}		

		try{

			db_merge ("tbl_home_points")
        	->key(array("gmid"=>$gmid,"invoice_id"=>$data['invoice_id']))
            ->fields ($fields)
            ->execute();

			drupal_set_message('Successfully collected');

		}catch(Exception $e){
			drupal_set_message($e->getMessage());
		}

	}

	function myatoz_member_point_redeem($parameter2){

		$data = array();

		$output = '';

		$data = atoz_get_member_by_id($parameter2);

		$member_level_arr = atoz_member_level();

		$output .= '<p class="error">'.$_SESSION['redeem_point'].'</p>' ;

		$output .= 
		'<h2>'.t('Redeem Gifts').'</h2>
		<table>
			<tr>
				<td>'.t('Name').'</td>
				<td>'.$data["name"].'</td>
			</tr>
			<tr>
				<td>'.t('Member ID').'</td>
				<td>'.$parameter2.'</td>
			</tr>
			<tr>
				<td>'.t('NRC No.').'</td>
				<td>'.$data["nrc"].'</td></tr>
			<tr>
				<td>'.t('Date Of Birth').'</td>
				<td>'.$data["dob"].'</td>
			</tr>
			<tr>
				<td>'.t('Phone Number').'</td>
				<td>'.$data["phone"].'</td>
			</tr>
			<tr>
				<td>'.t('Total Points Collected').'</td>
				<td>'.$data["current_point"].'</td>
			</tr>	
			<tr>
				<td>'.t('Member Level').'</td>
				<td>'.$member_level_arr[$data["member_level"]].'</td>
			</tr>		
		</table>';


		$contentForm = drupal_get_form('atoz_point_redeem_form',$parameter2,$data['current_point']);

		$output .= drupal_render($contentForm);

		return $output;

	}

	function atoz_point_redeem_form($form, &$form_state,$gmid,$current_point){

		$form = array('#id' => 'my-atoz-redeem-edit');

		$form['title'] = array (
	      	'#markup' => '',
	    );		

		$form['point_collect'] = array (
		    '#title' => t('Point Collected'),
	        '#type' => 'textfield',
	        '#default_value' => $current_point,
	        '#disabled' =>true,
	        '#required' =>true,
	    );

	    $gift_arr = myatoz_gifts_by_id($gmid);

	    $form["gift"] = array (
			'#title' => t('Gift'),
			'#type' => 'select',
			'#options' => $gift_arr,
	        '#required' =>true,
		);

		$form['gift_id'] = array (
	        '#type' => 'hidden',
	        '#attributes' => array('id'=>'redeem_gift_id'),
	    );

	    $form['redeem_point'] = array (
		    '#title' => t('Redeem Point'),
	        '#type' => 'textfield',
	        // '#required' =>true,
	    );

	    $form['gmid'] = array (
	        '#type' => 'hidden',
	        '#default_value' => $gmid,
	    );

	    $form['qty'] = array (
	        '#type' => 'hidden',
	        '#attributes' => array('id'=>'edit-qty'),
	    );

	    $form['save'] = array(
		    '#type' => 'submit',
		    '#value' => t('Redeem Now'),
		);	


	    return $form;
	}

	function atoz_point_redeem_form_validate($form, &$form_state){

		$current_point = $form_state['values']['point_collect'];

		$redeem_point = $form_state['values']['redeem_point'];

		if($current_point < $redeem_point){

			$_SESSION['redeem_point'] = t('Your redemption points are exceeded more than your available points');

			form_set_error('redeem_point', t('Your redemption points are exceeded more than your available points'));

		}else{
			unset($_SESSION['redeem_point']);
		}

	}

	function atoz_point_redeem_form_submit($form, &$form_state){

		global $user;

		$now = date('Y-m-d G:i:s');

		$data = $form_state['values'];

		$current_point = 0;

		$fields = array();
		$fields['grid'] = $data['gift_id'];
		$fields['gmid'] = $data['gmid'];
		$fields['redemption_point'] = $data['redeem_point'];
		$fields['created_date'] = $now;
		$fields['created_by'] = $user->uid;
		$fields['updated_date'] = $now;
		$fields['updated_by'] = $user->uid;

		$mfields = array();

		$current_point = $data['point_collect'] - $data['redeem_point'];

		$mfields['current_point'] = $current_point;

		$query = db_select('tbl_home_users_info','u');
		$query ->fields('u',array('city'));
		$query ->condition('uid',$user->uid);

		$result = $query ->execute()->fetchObject();

		try{
			db_insert("tbl_home_users_redeems")        	
            ->fields($fields)
            ->execute();

            db_update("tbl_home_members_info")
            ->condition('gmid',$data['gmid'])
            ->fields($mfields)
            ->execute();

            db_update("tbl_home_gift_detail")
            ->condition('grid',$data['gift_id'])
            ->condition('cid', $result->city)
            ->fields(array('qty' => $data['qty']))
            ->execute();

			drupal_set_message('Successfully Redeemption');

		}catch(Exception $e){
			drupal_set_message($e->getMessage());
		}

		drupal_goto('/atoz/member/point_redeem/'.$data['gmid']);

	}


?>