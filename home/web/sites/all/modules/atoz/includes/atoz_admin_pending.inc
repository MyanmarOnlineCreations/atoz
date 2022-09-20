<?php
	function myatoz_pending($parameter1=null,$parameter2=null){
		$output = '';

		if($parameter1 == 'view' && $parameter2){
			$contentForm = drupal_get_form('myatoz_pending_form',$parameter2);
			$output .= drupal_render($contentForm);
		}else{
			$output .= myatoz_pending_list();
		}

		return $output;
	}

	function myatoz_pending_list(){

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
		);

		$headerFields = array (
			"Name"      	   => "name",
	        "Membership Level" => "member_level",
	        "Action"		   => ""
		);

		$query = db_select('tbl_home_members_info','m');
		$query -> fields('m');
		$query -> condition("m.status","1001","!=");
		$result = $query->execute();

		$status_arr = atoz_member_status();

		$member_type_arr = atoz_member_types();

		$member_level_arr = atoz_member_level();

		if($result){

			foreach($result as $row){
				
				$list['name'] = $row->name;
				$list['member_level'] = $member_type_arr[$row->member_type] . ' / ' . $member_level_arr[$row->member_level];				
				$list['action'] = '<a href="/atoz/pending/view/'.$row->gmid.'">View</a>';

				$data[] = $list;				
			}
			
		}


		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$pager = atoz_admin_pager_by_query($pageQuery,"m", "gmid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header"> Pending <span class="myst-admin-title-total">('.$total.')</span></h2>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;

	}

	function myatoz_pending_form($form, &$form_state,$member_id){

		$data = array();

		$data = atoz_get_member_by_id($member_id);

		$form = array('#id' => 'my-atoz-pending-edit');

		$form['title'] = array (
	      	'#markup' => 'Pending Member',
	    );		

		$form['name'] = array (
		    '#title' => t('Name'),
	        '#type' => 'textfield',
	        '#default_value' => $data["name"],
	        '#required' =>true,
	        '#disabled' => true,
	    );

	    $form["dob"] = array (
			'#title' => t('Date Of Birth'),
			'#type' => 'date_popup',
			'#default_value' => $data["dob"],
			'#date_format' => 'Y-m-d',
			'#date_year_range' => '-100:+1',
	        '#required' =>true,
	        '#disabled' => true,
		);


	    $form['nrc'] = array (
		    '#title' => t('NRC Number'),
	        '#type' => 'textfield',
	        '#default_value' => $data["nrc"],
	        '#required' =>true,
	        '#disabled' => true,
	    );


	    $form['phone'] = array (
		    '#title' => t('Phone Number'),
	        '#type' => 'textfield',
	        '#default_value' => $data["phone"],
	        '#required' =>true,
	        '#disabled' => true,	        
	    );	   

	    $city_arr = atoz_division();

	    $form['city'] = array (
		    '#title' => t('City'),
	        '#type' => 'select',
	        '#options'=>$city_arr,
	        '#default_value' => $data["city"],
	        '#disabled' => true,
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
	        	'#disabled' =>true,	        	
		    );
	    }

	    $form['address'] = array (
		    '#title' => t('Address'),
	        '#type' => 'textarea',
	        '#default_value' => $data["address"],
	        '#required' =>true,
	        '#disabled' =>true,
	    );

	    $form['current_point'] = array (
		    '#title' => t('Current Point'),
	        '#type' => 'textfield',
	        '#default_value' => $data["current_point"],
	    );

	    $member_level_arr = atoz_member_level();

	    $form['member_level'] = array (
		    '#title' => t('Membership Level'),
	        '#type' => 'select',
	        '#options' => $member_level_arr,
	        '#default_value' => $data["member_level"],
	    );	    

	    $form['gmid'] = array(
	    	'#type' => 'hidden',
	    	'#default_value' => $member_id,
	    );


	    $form['save'] = array(
		    '#type' => 'submit',
		    '#value' => 'Approve',
		);

		$form['reject'] = array(
		    '#type' => 'submit',
		    '#value' => 'Reject',
		);

		return $form;

	}

	function myatoz_pending_form_validate($form, &$form_state){

	}

	function myatoz_pending_form_submit($form, &$form_state){

		global $user;

		$now = date('Y-m-d G:i:s');

		$data = $form_state['values'];

		$fields = array();

		if($form_state['values']['op'] == 'Approve'){

			$fields['current_point'] = $data['current_point'];
			$fields['member_level'] = $data['member_level'];
			$fields['updated_date'] = $now;
			$fields['updated_by'] = $user->uid;
			$fields['status'] = '1001'; //active
			try{
				db_merge ("tbl_home_members_info")
	        	->key(array("gmid"=>$data['gmid']))
	            ->fields ($fields)
	            ->execute();

	            db_merge ("users")
	        	->key(array("name"=>$data['gmid']))
	            ->fields (array('status'=>1))
	            ->execute();
				
	        	drupal_set_message('Successfully varified');

			}catch(Exception $e){
	        	 drupal_set_message($e->getMessage(), 'Err');
			}
		}else if($form_state['values']['op'] == 'Reject'){

			$fields['status'] = '1002'; //reject
			$fields['member_level'] = $data['member_level'];

			try{

				db_merge ("tbl_home_members_info")
	        	->key(array("gmid"=>$data['gmid']))
	            ->fields ($fields)
	            ->execute();
				
	        	drupal_set_message('Successfully reject');

			}catch(Exception $e){
	        	 drupal_set_message($e->getMessage(), 'Err');
			}

		}

		drupal_goto('/atoz/pending');

	}
	
?>