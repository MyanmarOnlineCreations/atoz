<?php
	function myatoz_rules($parameter1=null,$parameter2=null){
	   global $user;
  	   if (in_array('super admin', $user->roles)) {

		$output = '';

		if($parameter1 == 'new'){
			$contentForm = drupal_get_form('myatoz_rules_form');
			$output .= drupal_render($contentForm);
		}else if($parameter1 == 'edit' && $parameter2){
			$contentForm = drupal_get_form('myatoz_rules_form',$parameter2);
			$output .= drupal_render($contentForm);
		}else{
			$output .= myatoz_rules_list();	
		}

		return $output;
	   }
	}


	function myatoz_rules_list(){
		
		global $user;

		$output="";	
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
		$offset = atoz_admin_page_offset($rowsPerPage);		
		
    	$data = array();

		$sortOptions = array (
			"default"			=> "member_level",
			"member_level"		=> "",
			"spent_amount"  	=> "desc",
			"point"				=> "",
			
		);

		$styleArray = array (
			"",
			"",
			"",
			"",
			""	
		);

		$headerFields = array (
			t("Member Level")     => "member_level",
	        t("Spent Amount") 	  => "spent_amount",
	        t("Point")	  		  => "point",
	        t("Action") 		  => "",
		);

		$query = db_select('tbl_home_rules','r');
		$query -> fields('r');

		$query->range($offset, $rowsPerPage);

	    atoz_checksort ($sortOptions, $query);
	    
		$result = $query->execute();

		$member_level_arr = atoz_member_level();

		if($result){			
			foreach($result as $row){
				$list['member_level'] = $member_level_arr[$row->member_level];
				$list['spent'] = $row->spent_amount;
				$list['point'] = $row->point;
				$list['action'] = '<a href="/atoz/memberlayer_rules/edit/'.$row->ruleId.'">Edit</a>';

				$data[] = $list;				
			}			
		}

		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$pager = atoz_admin_pager_by_query($pageQuery,"r", "id", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		// $output .= '<a href="/atoz/memberlayer_rules/new"> + '.t('Add Rule').'</a>';


		$output .= '<h2 class="list_header">'. t('Rules') .'<span class="myst-admin-title-total">('.$total.')</span></h2>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;

	}

	function myatoz_rules_form($form, &$form_state, $ruleId = ''){

		$data = atoz_get_rule($ruleId);

		$form = array('#id' => 'my-atoz-outlet-edit');

		$form['title'] = array (
	      	'#markup' => t('New Rule'),
	    );		

	    $member_level_arr = atoz_member_level();

	    $form['member_level'] = array (
		    '#title' => t('Member Level'),
	        '#type' => 'select',
	        '#options'=>$member_level_arr,
	        '#default_value' => $data["member_level"],
	    );

	    $form['spent_amount'] = array (
		    '#title' => t('Spent Amount (MMK)'),
	        '#type' => 'textfield',
	        '#default_value' => $data["spent_amount"],
	    );

	    $form['point'] = array (
		    '#title' => t('Point'),
	        '#type' => 'textfield',
	        '#default_value' => $data["point"],
	    );	

	    $form['ruleId'] = array(
	    	'#type' => 'hidden',
	    	'#default_value' => $ruleId,
	    );   


	    $form['save'] = array(
		    '#type' => 'submit',
		    '#value' => t('Save'),
		);

		return $form;
	}

	function myatoz_rules_form_validate($form, &$form_state){

	}

	function myatoz_rules_form_submit($form, &$form_state){

		global $user;

		$now = date('Y-m-d h:i:s');

		$data = $form_state['values'];

		$fields = array();

		$alreadyExist = false;

		$member_level_arr = atoz_member_level();

		if($data['ruleId'] == ''){

			$ruleId = atoz_get_unique_profile_id('R');	

			$query = db_select('tbl_home_rules','h');
			$query -> fields('h',array('ruleId'));
			$query -> condition('member_level',$data['member_level']);
			$result = $query->execute()->fetchObject();

			if($result->ruleId != ''){
				$alreadyExist = true;
			}

		}else{
			$ruleId = $data['ruleId'];
		}			

		if(!$alreadyExist){

			$fields['member_level'] = $data['member_level'];
			$fields['spent_amount'] = $data['spent_amount'];
			$fields['point'] = $data['point'];
			$fields['created_date'] = $now;
			$fields['created_by'] = $user->uid;

			try{
				db_merge('tbl_home_rules')
		        ->key(array('ruleId' => $ruleId))
		        ->fields($fields)
		        ->execute();
		        drupal_set_message('Successfully save');

			}catch(Exception $e){
				drupal_set_message($e->getMessage());
			}

		}else{
			drupal_set_message($member_level_arr[$data['member_level']] . ' is already exist');
		}	


        drupal_goto('/atoz/memberlayer_rules');
	}

?>