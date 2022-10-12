<?php
	function myatoz_outlet($parameter1=null,$parameter2=null){
		$output = '';
		if($parameter1 == 'new'){
			$contentForm = drupal_get_form('myatoz_outlets_form');
			$output .= drupal_render($contentForm);
		}else if($parameter1 == 'edit' && $parameter2){
			$contentForm = drupal_get_form('myatoz_outlets_form',$parameter2);
			$output .= drupal_render($contentForm);
		}else{
			$output .= myatoz_outlets_list();
		}
		return $output;
	}

	function myatoz_outlets_list(){
		global $user;
		$output="";	
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
		$offset = atoz_admin_page_offset($rowsPerPage);		
		
    	$data = array();

		$sortOptions = array (
			"default"	=> "name",
			"name"		=> "desc",
			"township"	=> "",
			"city"		=> "",
			//"no_of_staff"		=> "",
			
		);

		$styleArray = array (
			"",
			"",
			"",
			""	
		);

		$headerFields = array (
			t("Outlet Name")      	=> "name",
	        t("Township") 			=> "township",
	        t("City")				=> "city",
				
		);

		if(in_array('super admin',$user->roles)){
			$headerFields[t('Action')] = "";
		}

		$query = db_select('tbl_home_outlets','o');
		$query -> fields('o');
		$query -> condition('status',1);

		$query->range($offset, $rowsPerPage);

	    atoz_checksort ($sortOptions, $query);

		$result = $query->execute();

		$city_arr = atoz_division();
		$township_arr = atoz_townships();

		if($result){
			$i = 0;
			foreach($result as $row){

				if(isset($row->township) && $row->township != '' && $row->township != null){
					$township = $row->township;
				}else{
					$township = '';
				}

				if(isset($row->city) && $row->city != '' && $row->city != null){
					$city = $row->city;
				}else{
					$city = '';
				}
				
				$list['name'] = $row->name;
				$list['township'] = $township_arr[$township];
				$list['city'] = $city_arr[$row->city];
				if(in_array('super admin',$user->roles)){
					$list['action'] = '<a href="/atoz/outlet/edit/'.$row->goid.'">Edit</a>';
				}

				//$list['no_of_staff'] = $row->name;

				$data[] = $list;
				
			}
			
		}

		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		if(in_array('super admin',$user->roles)){
			$output .= '<a href="/atoz/outlet/new"> + '. t('New Outlet').'</a>';
		}


		$pager = atoz_admin_pager_by_query($pageQuery,"o", "goid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header"> '. t('Outlets').' <span class="myst-admin-title-total">('.$total.')</span></h2>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;
	}

	function myatoz_outlets_form($form, &$form_state,$goid = ''){

		$data = array();

		$data = atoz_get_outlet_by_id($goid);

		if($goid == ''){
			$form_title = 'New Outlet';
			$submit_title = 'Create';
		}else{
			$form_title = 'Edit Outlet';
			$submit_title = 'Update';
		}

		$form = array('#id' => 'my-atoz-outlet-edit');

		$form['title'] = array (
	      	'#markup' => t($form_title),
	    );		

		$form['name'] = array (
		    '#title' => t('Name'),
	        '#type' => 'textfield',
	        '#default_value' => $data["name"],

	    );

	    $city_arr = atoz_division();

	    $form['city'] = array (
		    '#title' => t('Stage & Region'),
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
			'#default_value' => $data["township"]
		    );
	    }

	    $form['goid'] = array(
	    	'#type' => 'hidden',
	    	'#default_value' => $goid,
	    );
		
	    $form['save'] = array(
		    '#type' => 'submit',
		    '#value' => t($submit_title),
		);

		return $form;
	}

	function myatoz_outlets_form_validate($form, &$form_state){

	}

	function myatoz_outlets_form_submit($form, &$form_state){

		global $user;

		$now = date('Y-m-d h:i:s');

		$data = $form_state["values"];
		if($data["goid"] == ''){
			$goid = atoz_get_unique_profile_id('O');
		}else{
			$goid = $data["goid"];
		}
		$fields = array(
			"name" => $data['name'],			
			"status" => 1,
			"created_date" => $now,
			"created_by" => $user->uid
		);

		//if(isset($data['township'])){
			$fields["township"] = $data['township'.$data['city']];			
		//}

		if(isset($data['city'])){
			$fields["city"] = $data["city"];
		}

		try{

			db_merge ("tbl_home_outlets")
        	->key(array("goid"=>$goid))
            ->fields ($fields)
            ->execute();
			
        	drupal_set_message('Successfully Save');

		}catch(Exception $e){
        	 drupal_set_message($e->getMessage(), 'Err');
		}

		drupal_goto('/atoz/outlet');   		

	}


?>