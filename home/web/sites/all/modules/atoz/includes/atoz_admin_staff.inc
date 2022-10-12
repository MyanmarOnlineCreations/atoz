<?php
	function myatoz_staff($parameter1=null,$parameter2=null){
		$output = '';

		if($parameter1 == 'new'){

			$contentForm = drupal_get_form('myatoz_staff_form');
			$output .= drupal_render($contentForm);
			
		}else if($parameter1 == 'edit' && $parameter2){

			$contentForm = drupal_get_form('myatoz_staff_form',$parameter2);
			$output .= drupal_render($contentForm);
			
		}else{
			$output .= myatoz_staff_list();
		}

		return $output;
	}

	function myatoz_staff_list(){
		global $user;
		$output="";	
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
		$offset = atoz_admin_page_offset($rowsPerPage);		
		
    	$data = array();

		$sortOptions = array (
			"name"				=> "name",
			// "department"		=> "desc",
			"name"				=> "desc",	
			"rl.name"			=> "desc",	
			"outlet"			=> "desc",	
			"remark"			=> "",
			
		);

		$styleArray = array (			
			"",
			"",
			"",	
			""
		);

		$headerFields = array (
			t("Name")      		=> "name",
	        // t("Department") 	=> "department",
	        t("Role")			=> "rl.name",
	        t("Outlet")			=> "outlet",
	        t("Remark")			=> "remark"	,
	        t("Action")			=> "",
		);

		$query = db_select('tbl_home_users_info','u');
		$query ->join('users_roles','r','r.uid = u.uid');
		$query ->join('role','rl','r.rid = rl.rid');
		$query -> fields('u');
		$query -> addField('rl','name','role_name');

		$query->range($offset, $rowsPerPage);

	    atoz_checksort ($sortOptions, $query);

		$result = $query->execute();

		// $dep_arr = atoz_department();

		$outlet_arr = atoz_get_outlet();

		if($result){
			
			foreach($result as $row){				
				$list['name'] = $row->name;
				// $list['department'] = $dep_arr[$row->department];
				$list['role_name'] = $row->role_name;
				$list['outlet']	= $outlet_arr[$row->outlet];
				$list['remark'] = $row->remark;
				$list['action'] = '<a href="/atoz/staff/edit/'.$row->uid.'">Edit</a>';
				$data[] = $list;				
			}			
		}

		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$output .= '<a href="/atoz/staff/new"> + '.  t('New Staff').'</a>';


		$pager = atoz_admin_pager_by_query($pageQuery,"u", "guid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header"> '.t('Staff').' <span class="myst-admin-title-total">('.$total.')</span></h2>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;
	}

	function myatoz_staff_form($form, &$form_state, $uid = ''){

		$data = array();

		$data = atoz_get_user_by_id($uid);

		if($uid == ''){
			$form_title = t('New Staff');
			$submit_title = t('Create');
		}else{
			$form_title = t('Edit Staff');
			$submit_title = t('Update');
		}

		$form = array('#id' => 'my-atoz-staff-edit');

		$form['title'] = array (
	      	'#markup' => $form_title,
	    );

	    $form['name'] = array (
		    '#title' => t('Name'),
	        '#type' => 'textfield',
	        '#required' => true,
	        '#default_value' => $data["name"],
	    );		

		$form['email'] = array (
		    '#title' => t('Email Address'),
	        '#type' => 'textfield',
	        '#required' => true,
	        '#default_value' => $data["email"],
	    );

	    $form['phone'] = array (
		    '#title' => t('Phone Number'),
	        '#type' => 'textfield',
	        '#required' => true,	        
	        '#default_value' => $data["phone"],
	    );

	    $role_arr = atoz_get_role();

		$form['role'] = array (
		    '#title' => t('Role'),
	        '#type' => 'select',
	        '#options'=>$role_arr,
	        '#required' => true,	        
	        '#default_value' => $data["rid"],
	    );

	    $outlet_arr = atoz_get_outlet();

		$form['outlet'] = array (
		    '#title' => t('Outlet Name'),
	        '#type' => 'select',
	        '#options'=>$outlet_arr,
	        '#required' => true,	        
	        '#default_value' => $data["outlet"],
	    );

	    $city_arr = atoz_division();

	    $form['city'] = array (
		    '#title' => t('City'),
	        '#type' => 'select',
	        '#options'=>$city_arr,
	        '#default_value' => $data["city"],
	    );

	   for($i = 1; $i <= 15 ; $i++){

	    	$township_arr = atoz_townships($i);

		    $form['township'.$i] = array (
			    '#title' => t('Township'),
		        '#type' => 'select',
		        '#options'=>$township_arr,
		        '#default_value' => $data['township'],
		    );
	    }	    

	    if($uid == ''){
	    	$form['password'] = array (
			    '#title' => t('Password'),
		        '#type' => 'password',
		        '#required' => true,
		    );

		    $form['confirm_password'] = array (
			    '#title' => t('Confirmed Password'),
		        '#type' => 'password',
		        '#required' => true,
		    );
	    }

	    $form['remark'] = array (
		    '#title' => t('remark'),
	        '#type' => 'textarea',
	        '#default_value' => $data["remark"],
	    );

	    if($uid != ''){
	    	$form['reset_pwd'] = array (
				'#type' => 'fieldset',
				'#title' => t('Reset Password'),
				'#collapsible' => TRUE,
				'#collapsed' => FALSE,
			);

		    $form['reset_pwd']['password'] = array (
			    '#title' => t('Password'),
		        '#type' => 'password',
		    );

		    $form['reset_pwd']['confirm_password'] = array (
			    '#title' => t('Confirmed Password'),
		        '#type' => 'password',
		    );
	    }

	    $form['uid'] = array(
	    	'#type' => 'hidden',
	    	'#default_value' => $uid,
	    );

	    $form['guid'] = array(
	    	'#type' => 'hidden',
	    	'#default_value' => $data['guid'],
	    );

	    $form['save'] = array(
		    '#type' => 'submit',
		    '#value' => $submit_title,
		);

		return $form;
	}

	function myatoz_staff_form_validate($form, &$form_state){

		if ($form_state['values']['password'] !== $form_state['values']['confirm_password']) { 

			form_set_error('password', t('Password does not match'));

			form_set_error('confirm_password', t('Password does not match'));
		}

	}

	function myatoz_staff_form_submit($form , &$form_state){

		global $user;

		$now = date('Y-m-d G:i:s');

		$data = $form_state['values'];

		$fields = array();

		$uid = 0;

		$guid = $data['guid'];

		if($data["uid"] == ''){

			$guid = atoz_get_unique_profile_id('U');

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
		    	
;		        // creating staff account
		        $username = str_replace(' ','',$data['name']);
		        $email = $data['email'];
		        $details2["mail"] = $email;
		        $details2["name"] = $username;

		        try{
		        	$account = user_save($account,$details2);
		        }catch (Exception $e){
		        	print_r($e->getMessage());
		        }  		


		       db_merge('users_roles')
	            ->key(array("uid"=>$uid))
	            ->fields(array('rid'=>$data['role']))
	            ->execute();
		    }

		    $fields['created_date'] = $now;
			$fields['created_by'] = $user->uid;			

		}else{
			$uid = $data["uid"];			

			$existingUser = user_load($uid);

			// create an array of properties to update
			$existingUser->pass = $data['password'];

			user_save((object) array('uid' => $existingUser->uid), (array) $existingUser);

			 db_merge('users_roles')
            ->key(array("uid"=>$uid))
            ->fields(array('rid'=>$data['role']))
            ->execute();

			$fields['updated_date'] = $now;
			$fields['updated_by'] = $user->uid;		

		}

		$fields['guid'] = $guid;
		$fields['name'] = $data['name'];
		$fields['email'] = $data['email'];
		$fields['phone'] = $data['phone'];		
		$fields['city'] = $data['city'];
		$fields['township'] = $data['township'.$data['city']];
		$fields['outlet'] = $data['outlet'];
		$fields['remark'] = $data['remark'];


		if(isset($data['status'])){
			$fields['status'] = $data['status'];
		}

		try{
			db_merge ("tbl_home_users_info")
        	->key(array("uid"=>$uid))
            ->fields ($fields)
            ->execute();

        	drupal_set_message('Successfully Create Staff');

		}catch(Exception $e){
        	drupal_set_message($e->getMessage(), 'Err');
		}

		drupal_goto('/atoz/staff');
	}
?>