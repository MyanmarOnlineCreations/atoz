<?php

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
		$offset = 0;
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
    	$data = array();

		$sortOptions = array (
			"default"			=> "created_date",
			"name"				=> "",
			"created_date"  	=> "desc",
			"township"			=> "",
			"city"				=> "",			
		);

		$styleArray = array (
			"",
			"",
			"",
			"right",
			"",
			"",
			""	
		);

		$headerFields = array (
			"Customer Name"      	=> "name",
	        "Member ID" 			=> "gmid",
	        "Point Collect"		=> "current_point",
	        "Last Purchase Amount"	=> "amount",
	        "Status"				=> "status",
	        "Remark"				=> "",	        
	        "Action"				=> "",
		);

		$query = db_select('tbl_home_members_info','m');
		$query -> fields('m');

		if(isset($_SESSION[ATOZ_USER_ID_FILTER])){
			$query->condition('gmid',$_SESSION[ATOZ_USER_ID_FILTER]);
		}

		$result = $query->execute();

		$status_arr = atoz_member_status();

		if($result){

			foreach($result as $row){

				$list = array();					
				$list['name'] = $row->name;
				$list['gmid'] = $row->gmid;
				$list['current_point'] = number_format($row->current_point);	
				$list['amount']	 = number_format($row->lp_amount);		
				$list['status'] = $status_arr[$row->status];

				if(in_array('staff admin',$user->roles) && $row->status == "1001"){
					$list['remark'] = '<a href="/atoz/member/point_collect/'.$row->gmid.'">Collect Points</a> / <a href="/atoz/member/point_redeem/'.$row->gmid.'">Redeem Point</a>';
				}else{
					$list['remark'] = '';
				}

				$list['action'] = '<a href="/atoz/member/edit/'.$row->gmid.'">Edit</a>';


				$data[] = $list;
				
			}
			
		}


		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$output .= '<a href="/atoz/member/new"> + New Member</a>';


		$pager = atoz_admin_pager_by_query($pageQuery,"m", "gmid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header"> Members <span class="myst-admin-title-total">('.$total.')</span></h2>';


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

		$form = array('#id' => 'my-atoz-user-filter-form');
	

		$form['user_id'] = array (
		    '#title' => t('User ID'),
	        '#type' => 'textfield',
	        '#default_value' => $defaultUserIDFilter,
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
		if ($form_state["values"]["op"] == "Reset") {
			unset ($_SESSION[ATOZ_USER_ID_FILTER]);
			
		}else{
			
			$userIdFilter = $form_state["values"]["user_id"];
			
			if (!$userIdFilter) {
				unset ($_SESSION[ATOZ_USER_ID_FILTER]);
			}
			else {
				$_SESSION[ATOZ_USER_ID_FILTER] = $userIdFilter;
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
	      	'#markup' => $form_title,
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
	        '#required' =>true,


	    );


	    $form['phone'] = array (
		    '#title' => t('Phone Number'),
	        '#type' => 'textfield',
	        '#default_value' => $data["phone"],
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
		    '#title' => t('City'),
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
	        '#required' =>true,	        
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
		    '#value' => $submit_title,
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

		$now = date('Y-m-d G:i:s');

		$data = $form_state["values"];

		$fields = array();

		$uid = $data['uid'];

		if($data["gmid"] == ''){
			$gmid = atoz_get_unique_profile_id('M');

			$account = new stdClass();

		    $account->is_new = true;
		    // temporary name
		    $username = rand(1000, 9999) . $now;    

		    $details["mail"] = $username;
		    $details["name"] = $username;
		    $details["status"] = 0; //default status is block;
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

		    $fields['created_date'] = $now;
			$fields['created_by'] = $user->uid;

		}else{
			$gmid = $data["gmid"];
		}

		$fields = array(
			"uid" => $uid,
			"name" => $data['name'],
			"dob" => $data['dob'],
			"nrc" => $data['nrc'],
			"phone" => $data['phone'],
			"current_point" => $data['current_point'],
			"member_type" => $data['member_type'], 
			"township" => $data['township'.$data['city']],
			"city" => $data['city'],
			"address" => $data['address'],
		);

		if(isset($data['status'])){
			$fields['status'] = $data['status'];	

			if($data['status'] == '1001'){
				db_merge ("users")
	        	->key(array("uid"=>$uid))
	            ->fields (array('status'=>1))
	            ->execute();
			}		
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
		'<h2>Collect Points</h2>
		<table>
			<tr>
				<td>Name</td>
				<td>'.$data["name"].'</td>
			</tr>
			<tr>
				<td>Member ID</td>
				<td>'.$parameter2.'</td>
			</tr>
			<tr>
				<td>NRC No.</td>
				<td>'.$data["nrc"].'</td></tr>
			<tr>
				<td>Date Of Birth</td>
				<td>'.$data["dob"].'</td>
			</tr>
			<tr>
				<td>Phone Number</td>
				<td>'.$data["phone"].'</td>
			</tr>
			<tr>
				<td>Total Points Collected</td>
				<td>'.$data["current_point"].'</td>
			</tr>	
			<tr>
				<td>Member Level</td>
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
		    '#value' => 'Collect Now',
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

		$output .= 
		'<h2>Redeem Gifts</h2>
		<table>
			<tr>
				<td>Name</td>
				<td>'.$data["name"].'</td>
			</tr>
			<tr>
				<td>Member ID</td>
				<td>'.$parameter2.'</td>
			</tr>
			<tr>
				<td>NRC No.</td>
				<td>'.$data["nrc"].'</td></tr>
			<tr>
				<td>Date Of Birth</td>
				<td>'.$data["dob"].'</td>
			</tr>
			<tr>
				<td>Phone Number</td>
				<td>'.$data["phone"].'</td>
			</tr>
			<tr>
				<td>Total Points Collected</td>
				<td>'.$data["current_point"].'</td>
			</tr>	
			<tr>
				<td>Member Level</td>
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
	        '#required' =>true,
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
		    '#value' => 'Redeem Now',
		);	


	    return $form;
	}

	function atoz_point_redeem_form_validate($form, &$form_state){

	}

	function atoz_point_redeem_form_submit($form, &$form_state){

		global $user;

		$now = date('Y-m-d G:i:s');

		$data = $form_state['values'];

		$current_point = 0;

		$fields = array();
		$fields['grid'] = $data['gift_id'];
		$fields['gmid'] = $data['gmid'];
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

	}
?>