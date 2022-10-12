<?php
	function myatoz_redemption($parameter1=null,$parameter2=null,$parameter3=null){
	   global $user;
  	   if (in_array('super admin', $user->roles)) {

		$output = '';
		if ($parameter1 == "edit" && $parameter2) {
	           $contentForm = drupal_get_form ("myatoz_gifts_form", $parameter2);
	           $output .= drupal_render ($contentForm);
	    	}else if ($parameter1 == "detail" && $parameter2) {
	           $output .= myatoz_gift_detail_list($parameter2);
	    	}else if($parameter1 == 'new'){
		   $contentForm = drupal_get_form('myatoz_gifts_form');
		   $output .= drupal_render($contentForm);
		}else if($parameter1 == 'new_detail'){
		   $contentForm = drupal_get_form('myatoz_giftsdetail_form',$parameter2);
		   $output .= drupal_render($contentForm);
		}else if($parameter1 == 'edit_detail'){
		   $contentForm = drupal_get_form('myatoz_giftsdetail_form',$parameter2,$parameter3);
		   $output .= drupal_render($contentForm);
		} else{
        		$output .= myatoz_gift_list();
       		}
		return $output;
	    }
	}

	function myatoz_gift_list()
	{
		global $user;
		$output="";	
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
		$offset = atoz_admin_page_offset($rowsPerPage);		
		
    	 	$data = array();

		$sortOptions = array (
			"default"	=> "gift_name",
			"gift_name"	=> "desc",
			"grid"  	=> "desc",
			"gpoint"	=> "desc",
			
		);

		$styleArray = array (
			"",
			"",
			"",
			""	
		);

		$headerFields = array (
			t("Item Name" )     	=> "gift_name",
      t("Item ID") 			=> "grid",
     t( "Point Value")		=> "gpoint",
      t("Action")			=> ""	
		);

		$query = db_select('tbl_home_gift','g');
		$query -> fields('g');
		$query -> condition('g.type',1);
		$query->range($offset, $rowsPerPage);

	  atoz_checksort ($sortOptions, $query);

		$result = $query->execute();


		if($result){
			foreach($result as $row){
				$list['gift_name'] = $row->gift_name;
				$list['grid'] = $row->grid;
				$list['gpoint'] = $row->gpoint;
				$list['action'] = "<a href='/atoz/redemption/detail/".$row->grid."'>Detail </a>|| <a href='/atoz/redemption/edit/".$row->grid."'>Edit </a>";
				$data[] = $list;
			}
		}

		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$output .= '<a href="/atoz/redemption/new"> + '.t('New Gift').'</a>';


		$pager = atoz_admin_pager_by_query($pageQuery,"g", "grid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header"> '.t("Redemption List").' <span class="myst-admin-title-total">('.$total.')</span></h2>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;
	}

	function myatoz_gifts_form($form, &$form_state, $grid=""){

		$form = array('#id' => 'atoz_gift_form');

		$data = array (
	        "gift_name"   => "",
	        "gpoint"      => "",
	        "grid"        => "",
	    );

	    if($grid){
	    	$query = db_select("tbl_home_gift", "g");
		    $query->fields ("g");
		    $query->condition("g.grid", $grid);
		    //echo $query;exit();
		    $result = $query->execute ();
		    foreach ($result as $row) {
		    	$data["gift_name"]     = $row->gift_name;
		    	$data["gpoint"]     = $row->gpoint;
		    	$data["grid"]     = $grid;
		    }
	    }

		$form['title'] = array (
	      	'#markup' => t('Gift Item Info'),
	    );		

		$form['gift_name'] = array (
		    '#title' => t('Item Name'),
	        '#type' => 'textfield',
	        '#default_value' => $data["gift_name"],

	    );
		$form['gpoint'] = array (
		    '#title' => t('Point Value'),
	        '#type' => 'textfield',
	        '#default_value' => $data["gpoint"],

	    );
	    $form['grid'] = array (
            '#type' => 'hidden',
            '#value' => $grid
        );

	    $form['submit'] = array (
            '#type' => 'submit',
            '#weight' => 98,
            '#value' => t('Save')
        );

        $form['back'] = array(
          '#type' => 'button',
          '#value' => t('Back'),
          '#attributes' => array(
            'onclick' => 'window.history.back();return false;',
          ),
        );

		return $form;
	}

	function myatoz_gifts_form_validate($form, &$form_state){

	}

	function myatoz_gifts_form_submit($form, &$form_state){

		global $user;

		$now = date('Y-m-d h:i:s');

		$data = $form_state["values"];

		
 		$grid = $data['grid'];
		$fields = array(
			"gift_name"    => $data['gift_name'],
			"grid" 		   => $data['grid'],
			"gpoint" 	   => $data["gpoint"],
			"updated_date" => $now,
			"updated_by"   => $user->uid,
		);
		if (!$grid) {
	        $grid = atoz_get_unique_giftitem_id('G');

	        $fields["grid"] = $grid;
	        $fields["created_by"]    = $user->uid;
	        $fields["created_date"] = $now;
	    }
		try{

			db_merge ("tbl_home_gift")
        	->key(array("grid"=>$grid))
            ->fields ($fields)
            ->execute();
			
        	drupal_set_message('Successfully Save');

		}catch(Exception $e){
        	 drupal_set_message($e->getMessage(), 'Error');
		}

		drupal_goto('/atoz/redemption');   		

	}
	function myatoz_gift_detail_list($grid){

		$gift_name = "";
		$point = "0";
    	$query = db_select("tbl_home_gift", "g");
	    $query->fields ("g");
	    $query->condition("g.grid", $grid);
	    $result = $query->execute ();
	    foreach ($result as $row) {
	    	$gift_name     = $row->gift_name;
	    	$gpoint     = $row->gpoint;
	    	$grid    = $grid;
	    }
	    $output = '<h2 class="list_header">'.t('Gift Details').'</h2>';
	    $output .= "<p>".t('Item Name')." : ".$gift_name."</p>";
	    $output .= "<p>".t('Item ID')." : ".$grid."</p>";
	    $output .= "<p>".t('Point Value')." : ".$gpoint."</p>";

		$offset = 0;
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
    	$data = array();

		$sortOptions = array (
			"default"	=> "created_date",
			"qty"	=> "",
			"cid"  	=> "",
			"goid"	=> "",
			
		);

		$styleArray = array (
			"",
			"",
			"",
			""	
		);

		$headerFields = array (
			t("Stocks")      	=> "qty",
	        t("City") 			=> "",
	        t("Outlet")		=> "",
	        t("Action")			=> ""	
		);

		$query = db_select('tbl_home_gift_detail','gd');
		$query -> fields('gd');
		$query->condition("gd.grid",$grid);
		$result = $query->execute();
      	 
      	$city_arr = atoz_division();
		$outlet_arr = atoz_get_outlet();
		if($result){
			foreach($result as $row){
				$list['qty'] = $row->qty;
				$list['city'] = $city_arr[$row->cid];
				$list['outlet'] = $outlet_arr[$row->goid];
				$list['action'] = "<a href='/atoz/redemption/edit_detail/".$grid."/".$row->gdid."'>Edit </a>";
				$data[] = $list;
			}
		}

		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$output .= '<a href="/atoz/redemption/new_detail/'.$grid.'"> +'. t("New Gift Detail").'</a>';


		$pager = atoz_admin_pager_by_query($pageQuery,"gd", "gdid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header"> '. t("Detail List").' <span class="myst-admin-title-total">('.$total.')</span></h2>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;
	    return $output;
	    
	}

	function myatoz_giftsdetail_form($form, &$form_state, $grid="", $gdid=""){

		$form = array('#id' => 'atoz_gift_detail_form');

		$data = array (
	        "stock"   => "",
	        "city"      => "",
	        "outlet"        => "",
	        "grid"        => "",
	        "gdid"        => "",
	    );

	    if($gdid){
	    	$query = db_select("tbl_home_gift_detail", "gd");
		    $query->fields ("gd");
		    $query->condition("gd.gdid", $gdid);
		    //echo $query;exit();
		    $result = $query->execute ();
		    foreach ($result as $row) {
		    	$data["stock"]     = $row->qty;
		    	$data["city"]     = $row->cid;
		    	$data["outlet"]   = $row->goid;
		    	$data["grid"]     = $row->grid;
		    	$data["gdid"]     = $gdid;
		    }
	    }

		$form['title'] = array (
	      	'#markup' => t('Gift Detail Info'),
	    );		

		$form['stock'] = array (
		    '#title' => t('Stock'),
	        '#type' => 'textfield',
	        '#default_value' => $data["stock"],

	    );
	    $city_arr = atoz_division();
	    $form['city'] = array (
		    '#title' => t('City'),
	        '#type' => 'select',
	        '#options'=>$city_arr,
	        '#default_value' => $data["city"],

	    );

	    $outlet_arr = atoz_get_outlet();
	    $form['outlet'] = array (
		    '#title' => t('Outlet'),
	        '#type' => 'select',
	        '#options'=>$outlet_arr,
	        '#default_value' => $data["outlet"],
	    );

	     $form['grid'] = array (
            '#type' => 'hidden',
            '#value' => $grid
        );
	      $form['gdid'] = array (
            '#type' => 'hidden',
            '#value' => $gdid
        );

	    $form['submit'] = array (
            '#type' => 'submit',
            '#weight' => 98,
            '#value' => t('Save')
        );

        $form['back'] = array(
          '#type' => 'button',
          '#value' => t('Back'),
          '#attributes' => array(
            'onclick' => 'window.history.back();return false;',
          ),
        );

		return $form;
	}

	function myatoz_giftsdetail_form_validate($form, &$form_state){

	}

	function myatoz_giftsdetail_form_submit($form, &$form_state){

		global $user;

		$now = date('Y-m-d G:i:s');

		$data = $form_state["values"];

		
 		$gdid = $data['gdid'];
 		$grid = $data["grid"];
		$fields = array(
			"qty"   => $data['stock'],
			"cid" 	=> $data['city'],
			"goid" 	=> $data["outlet"],
			"grid" 	=> $data["grid"],
			"updated_date" => $now,
			"updated_by"   => $user->uid,
		);
		if (!$gdid) {
	        $fields["created_by"]    = $user->uid;
	        $fields["created_date"] = $now;
	        try{
				db_insert ("tbl_home_gift_detail")
	            ->fields ($fields)
	            ->execute();
				
	        	drupal_set_message('Successfully Save');

			}catch(Exception $e){
	        	 drupal_set_message($e->getMessage(), 'Error');
			}
	        
	    }else{
	    	try{

				db_merge ("tbl_home_gift_detail")
	        	->key(array("gdid"=>$gdid))
	            ->fields ($fields)
	            ->execute();
				
	        	drupal_set_message('Successfully Update');

			}catch(Exception $e){
	        	 drupal_set_message($e->getMessage(), 'Error');
			}
	    }
		

		drupal_goto('atoz/redemption/detail/'.$grid);   		

	}
?>