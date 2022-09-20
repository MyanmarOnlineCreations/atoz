<?php
	function myatoz_outlet($parameter1=null,$parameter2=null){
		$output = '';
		if($parameter1 == 'new'){
			$contentForm = drupal_get_form('myatoz_outlets_form');
			$output .= drupal_render($contentForm);
		}else{
			$output .= myatoz_outlets_list();
		}
		return $output;
	}

	function myatoz_outlets_list(){
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
			//"no_of_staff"		=> "",
			
		);

		$styleArray = array (
			"",
			"",
			"",
			"",
			""	
		);

		$headerFields = array (
			"Outlet Name"      	=> "name",
	        "Township" 			=> "township",
	        "City"				=> "city",
			//"Number of Staff"  	=> "no_of_staff",	
		);

		$query = db_select('tbl_home_outlets','o');
		$query -> fields('o');
		$query -> condition('status',1);
		$result = $query->execute();

		$city_arr = atoz_division();
		$township_arr = atoz_townships();

		if($result){
			$i = 0;
			foreach($result as $row){
				
				$list['name'] = $row->name;
				$list['township'] = $township_arr[$row->township];
				$list['city'] = $city_arr[$row->city];
				//$list['no_of_staff'] = $row->name;

				$data[] = $list;
				
			}

			
		}

		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$output .= '<a href="/atoz/outlet/new"> + New Outlet</a>';


		$pager = atoz_admin_pager_by_query($pageQuery,"o", "goid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header"> Outlets <span class="myst-admin-title-total">('.$total.')</span></h2>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;
	}

	function myatoz_outlets_form($form, &$form_state){

		$form = array('#id' => 'my-atoz-outlet-edit');

		$form['title'] = array (
	      	'#markup' => 'New Outlet',
	    );		

		$form['name'] = array (
		    '#title' => t('Name'),
	        '#type' => 'textfield',
	        // '#default_value' => $data["division"],

	    );

	    $city_arr = atoz_division();

	    $form['city'] = array (
		    '#title' => t('City'),
	        '#type' => 'select',
	        '#options'=>$city_arr,
	        // '#default_value' => $data["division"],

	    );

	   for($i = 1; $i <= 15 ; $i++){

	    	$township_arr = atoz_townships($i);

		    $form['township'.$i] = array (
			    '#title' => t('Township'),
		        '#type' => 'select',
		        '#options'=>$township_arr,
		    );
	    }


	    $form['save'] = array(
		    '#type' => 'submit',
		    '#value' => 'Create',
		);

		return $form;
	}

	function myatoz_outlets_form_validate($form, &$form_state){

	}

	function myatoz_outlets_form_submit($form, &$form_state){

		global $user;

		$now = date('Y-m-d G:i:s');

		$data = $form_state["values"];

		$goid = atoz_get_unique_profile_id('O');

		$fields = array(
			"name" => $data['name'],
			"township" => $data['township'],
			"city" => $data["city"],
			"status" => 1,
			"created_date" => $now,
			"created_by" => $user->uid
		);

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