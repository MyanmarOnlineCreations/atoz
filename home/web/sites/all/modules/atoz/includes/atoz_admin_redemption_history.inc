<?php
	function myatoz_redemption_history(){

		$output = '';
		$contentForm = drupal_get_form('myatoz_redemption_history_filter_form');
		$output .= drupal_render($contentForm);
		$output .= myatoz_redemption_history_list();

		return $output;
	}

	function myatoz_redemption_history_list(){

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
			"gmid"				=> "",
			"gift_name"			=> "",
			"gift_point"		=> "",
			
		);

		$styleArray = array (
			"",
			"",
			"",
			"right",
			"",
			"",
		);

		$headerFields = array (
			"Date" 			=> 'created_date',
			"Member ID" 	=> "gmid",	
			"Name" 			=> "name",	        
	        "Gift Name"		=> "gift_name",
	        "Gift Point"	=> "gpoint",
		);

		$query = db_select('tbl_home_users_redeems','r');
		$query ->join('tbl_home_members_info','m','r.gmid = m.gmid');
		$query ->join('tbl_home_gift','g','r.grid = g.grid');
		$query -> fields('r',array('gmid','created_date'));
		$query -> fields('g',array('gift_name','gpoint'));
		$query -> fields('m',array('name'));
		$query -> condition('r.created_by',$user->uid);

		if(isset($_SESSION[ATOZ_REDEEM_MEMBER_TIER_FILTER])){
			$query ->condition('m.member_level',$_SESSION[ATOZ_REDEEM_MEMBER_TIER_FILTER]);
		}

		if(isset($_SESSION[ATOZ_REDEEM_DATE_FILTER])){
			$query ->condition('r.created_date' ,'%'.$_SESSION[ATOZ_REDEEM_DATE_FILTER].'%','like');
		}

		$result = $query->execute();

		if($result){
			foreach($result as $row){
				$list['created_date'] = date('Y-m-d g:i A', strtotime($row->created_date));
				$list['name'] = $row->name;
				$list['gmid'] = $row->gmid;
				$list['gift_name'] = $row->gift_name;
				$list['gift_point'] = $row->gpoint;

				$data[] = $list;
			}

		}

		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;


		$pager = atoz_admin_pager_by_query($pageQuery,"r", "grid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header"> Redemptions <span class="myst-admin-title-total">('.$total.')</span></h2>';


		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;

	}

	function myatoz_redemption_history_filter_form($form, &$form_state){

		if (isset ($_SESSION[ATOZ_REDEEM_DATE_FILTER])) {
	        $defaultRDateFilter = $_SESSION[ATOZ_REDEEM_DATE_FILTER];
	    }
	    else {
	        $defaultRDateFilter = "";
	    } 

	    if (isset($_SESSION[ATOZ_REDEEM_MEMBER_TIER_FILTER])) {
	        $defaultMemberTierFilter = $_SESSION[ATOZ_REDEEM_MEMBER_TIER_FILTER];
	    }
	    else {
	        $defaultMemberTierFilter = " ";
	    } 	    

		$form = array('#id' => 'my-atoz-redeem-filter-form');

		$member_level_arr = atoz_member_level();	

	    $form['member_tier'] = array (
		    '#title' => t('Membership Tier'),
	        '#type' => 'select',
	        '#options' => $member_level_arr,
	        '#default_value' => $defaultMemberTierFilter,
	    );	

		$form["redeem_date"] = array (
			'#title' => t('Date'),
			'#type' => 'date_popup',
			'#default_value' => $defaultRDateFilter,
			'#date_format' => 'Y-m-d',
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

	function myatoz_redemption_history_filter_form_validate($form, &$form_state){

	}

	function myatoz_redemption_history_filter_form_submit($form, &$form_state){
		if ($form_state["values"]["op"] == "Reset") {
			unset ($_SESSION[ATOZ_REDEEM_DATE_FILTER]);
			unset ($_SESSION[ATOZ_REDEEM_MEMBER_TIER_FILTER]);
			
		}else{
			
			$dateFilter = $form_state["values"]["redeem_date"];
			
			if (!$dateFilter) {
				unset ($_SESSION[ATOZ_REDEEM_DATE_FILTER]);
			}
			else {
				$_SESSION[ATOZ_REDEEM_DATE_FILTER] = $dateFilter;
			}	

			$member_tier_filter = $form_state['values']['member_tier'];	

			if (!$member_tier_filter) {
				unset ($_SESSION[ATOZ_REDEEM_MEMBER_TIER_FILTER]);
			}
			else {
				$_SESSION[ATOZ_REDEEM_MEMBER_TIER_FILTER] = $member_tier_filter;
			}
			
		}

	}
	

	
?>