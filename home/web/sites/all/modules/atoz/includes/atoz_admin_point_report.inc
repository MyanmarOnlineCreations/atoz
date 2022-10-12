<?php 

	function myatoz_point_report($parameter1=null,$parameter2=null){
		$output = '';

		if($parameter1 == 'csv'){
			$output .= myatoz_point_report_download();
		}else{
			$contentForm = drupal_get_form('myatoz_point_report_filter_form');
			$output .= drupal_render($contentForm);
			$output .= myatoz_point_report_list();
		}
		
		return $output;
	}

	function myatoz_point_report_list(){

		global $user;

		$output="";	
		$link = $_SERVER["REQUEST_URI"];
		$rowsPerPage = 100;
		$dataPerRow = 1;
		$offset = atoz_admin_page_offset($rowsPerPage);		

    	$data = array();

		$sortOptions = array (
			"default"			=> "p.created_date",
			"p.created_date"  	=> "desc",
			"p.gmid"			=> "desc",
			"member_level"		=> "desc",
			"gpoint"			=> "desc",			
			"outlet"			=> "desc",
			"p.point"			=> "desc",	
			"p.invoice_id"		=> "desc",
			"p.amount"			=> "desc",		
			"member_type"		=> "desc",		

		);

		$styleArray = array (
			"",
			"",
			"",
			"right",
			"",
			"",	
			"",
			""
		);

		$headerFields = array (			
	        t("Date")			=> "p.created_date",
	        t("Member ID" )    	=> "p.gmid",
	        t("Invoice ID")		=> "p.invoice_id",
	        t("Amount")			=> "p.amount",
	        t("Collected Point")=> "p.point",
	        t("Outlet")			=> "outlet",
	        t("Member Tier")   	=> "member_level",
	        t("Member Type")   	=> "member_type",

		);

		$query = db_select('tbl_home_points','p');
		$query ->leftjoin('tbl_home_members_info','m','p.gmid = m.gmid');
		$query ->leftjoin('tbl_home_users_info','i','p.created_by = i.uid');
		$query ->fields('i',array('outlet'));
		$query ->fields('p',array('gmid','created_date','point','invoice_id','amount'));
		$query ->fields('m',array('member_level','member_type'));

		if(isset($_SESSION[ATOZ_POINT_MID_FILTER])){
			$query->condition('p.gmid', trim($_SESSION[ATOZ_POINT_MID_FILTER]));
		}

		if(isset($_SESSION[ATOZ_POINT_MEMBER_LEVEL_FILTER])){
			$query->condition('m.member_level',$_SESSION[ATOZ_POINT_MEMBER_LEVEL_FILTER]);
		}

		if(isset($_SESSION[ATOZ_POINT_DATE_FILTER])){
			$query->where(" date(p.created_date) = '" . $_SESSION[ATOZ_POINT_DATE_FILTER] ."'" );
		}

		if(isset($_SESSION[ATOZ_POINT_OUTLET_FILTER])){
			$query->condition('outlet',$_SESSION[ATOZ_POINT_OUTLET_FILTER]);
		}

		if(isset($_SESSION[ATOZ_POINT_MEMBER_TYPE])){
			$query->condition('member_type', $_SESSION[ATOZ_POINT_MEMBER_TYPE]);
		}

		$query->range($offset, $rowsPerPage);

	    atoz_checksort ($sortOptions, $query);

		$result = $query->execute();

		$outlet_arr = atoz_get_outlet();

		$member_level_arr = atoz_member_level();

		$member_type_arr = atoz_member_types();

		if($result){

			foreach($result as $row){

				if(isset($row->outlet) && $row->outlet != null && $row->outlet != ''){
					$outlet = $outlet_arr[$row->outlet];
				}else{
					$outlet = '';
				}

				$list['date'] = date('d-M-y',strtotime($row->created_date));
				$list['gmid'] = $row->gmid;
				$list['invoice_id'] = $row->invoice_id;
				$list['amount']	 = number_format($row->amount);
				$list['gpoint'] = number_format($row->point);
				$list['outlet'] = $outlet;
				$list['member_level'] = $member_level_arr[$row->member_level];
				$list['member_type'] = $member_type_arr[$row->member_type];

				$data[] = $list;
			}
		}

		// Member ID/Invoice ID/Amount/Point/Outlet

		// print_r($data);exit();	

		$header = atoz_generateHeader ($headerFields, $sortOptions);

		$pageQuery = $query;

		$pager = atoz_admin_pager_by_query($pageQuery,"p", "pid", $dataPerRow, $rowsPerPage, $currentPage, $offset, $total);

		$output .= '<h2 class="list_header">'. t('Point Management Report') .' <span class="myst-admin-title-total">('.$total.')</span></h2>';

		$output .= '<a class="downlod-csv" href="/atoz/pointreport/csv">'.t('Download CSV').'</a>';

		$output .=  $pager . atoz_admin_create_table ("home-admin-table", $header, $data, $styleArray) . $pager;

		return $output;
	}

	function myatoz_point_report_filter_form($form, &$form_state){

		if (isset ($_SESSION[ATOZ_POINT_DATE_FILTER])) {
	        $defaultDateFilter = $_SESSION[ATOZ_POINT_DATE_FILTER];
	    }
	    else {
	        $defaultDateFilter = "";
	    } 	

	    if (isset ($_SESSION[ATOZ_POINT_MEMBER_LEVEL_FILTER])) {
	        $defaultMemberLevelFilter = $_SESSION[ATOZ_POINT_MEMBER_LEVEL_FILTER];
	    }
	    else {
	        $defaultMemberLevelFilter = "";
	    } 	

	    if (isset ($_SESSION[ATOZ_POINT_MID_FILTER])) {
	        $defaultMemberIdFilter = $_SESSION[ATOZ_POINT_MID_FILTER];
	    }
	    else {
	        $defaultMemberIdFilter = "";
	    } 

	    if(isset($_SESSION[ATOZ_POINT_OUTLET_FILTER])){
	    	$defaultPointOutletFilter = $_SESSION[ATOZ_POINT_OUTLET_FILTER];
	    }else{
	    	$defaultPointOutletFilter = "";
	    }

	    if(isset($_SESSION[ATOZ_POINT_MEMBER_TYPE])){
	    	$defaultPointMemberType = $_SESSION[ATOZ_POINT_MEMBER_TYPE];
	    }else{
	    	$defaultPointMemberType = "";
	    }

		$form = array('#id' => 'my-atoz-point-report-filter-form');
	

		$form['filter_date'] = array (
		    '#title' => t('Date'),
	        '#type' => 'date_popup',
	        '#date_format' => 'Y-m-d',
	        '#default_value' => $defaultDateFilter,
	    );
	    
	    $form['member_id'] = array (
		    '#title' => t('Member ID'),
	        '#type' => 'textfield',
	        '#default_value' => $defaultMemberIdFilter,
	    );

	    $member_level_arr = atoz_member_level();

	    $form['member_level'] = array (
		    '#title' => t('Member Tier'),
	        '#type' => 'select',
	        '#options' => $member_level_arr,
	        '#default_value' => $defaultMemberLevelFilter,
	    );	    

	    $member_type_arr = atoz_member_types();

	    $form['member_type'] = array (
		    '#title' => t('Member Type'),
	        '#type' => 'select',
	        '#options' => $member_type_arr,
	        '#default_value' => $defaultPointMemberType,
	    );

	    $outlet_arr = atoz_get_outlet();
		
		$form['outlet'] = array (
		    '#title' => t('Outlet'),
	        '#type' => 'select',
	        '#options' => $outlet_arr,
	        '#default_value' => $defaultPointOutletFilter,
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

	function myatoz_point_report_filter_form_validate($form, &$form_state){

	}

	function myatoz_point_report_filter_form_submit($form, &$form_state){

		if ($form_state["values"]["op"] == t("Reset")) {

			unset($_SESSION[ATOZ_POINT_MEMBER_LEVEL_FILTER]);
			unset($_SESSION[ATOZ_POINT_DATE_FILTER]);
			unset($_SESSION[ATOZ_POINT_MID_FILTER]);
			unset($_SESSION[ATOZ_POINT_OUTLET_FILTER]);
			unset($_SESSION[ATOZ_POINT_MEMBER_TYPE]);
			
		}else{
			
			$dateFilter = $form_state["values"]["filter_date"];
			$memberLevelFilter = $form_state["values"]["member_level"];
			$memberIdFilter = $form_state["values"]["member_id"];
			$memberTypeFilter = $form_state["values"]["member_type"];
			$outletFilter = $form_state["values"]["outlet"];
			
			if (!$dateFilter) {
				unset ($_SESSION[ATOZ_POINT_DATE_FILTER]);
			}
			else {
				$_SESSION[ATOZ_POINT_DATE_FILTER] = $dateFilter;
			}	

			if (!$memberLevelFilter) {
				unset ($_SESSION[ATOZ_POINT_MEMBER_LEVEL_FILTER]);
			}
			else {
				$_SESSION[ATOZ_POINT_MEMBER_LEVEL_FILTER] = $memberLevelFilter;
			}

			if (!$memberIdFilter) {
				unset ($_SESSION[ATOZ_POINT_MID_FILTER]);
			}
			else {
				$_SESSION[ATOZ_POINT_MID_FILTER] = $memberIdFilter;
			}	

			if(!$memberTypeFilter){
				unset ($_SESSION[ATOZ_POINT_MEMBER_TYPE]);
			}else{
				$_SESSION[ATOZ_POINT_MEMBER_TYPE] = $memberTypeFilter;
			}

			if(!$outletFilter){
				unset ($_SESSION[ATOZ_POINT_OUTLET_FILTER]);
			}else{
				$_SESSION[ATOZ_POINT_OUTLET_FILTER] = $outletFilter;
			}
			
		}

	}

	function myatoz_point_report_download(){

		set_time_limit (0);

	    $header = array ("Date","Member ID","Invoice ID","Amount","Collected Point","Outlet","Member Tier");

	    header('Content-type: application/csv');
	    header ("Content-Disposition: attachment; filename=point_management_report.csv");
	    echo implode (",", $header) . "\n";

	    $query = db_select('tbl_home_points','p');
		$query ->leftjoin('tbl_home_members_info','m','p.gmid = m.gmid');
		$query ->leftjoin('tbl_home_users_info','i','p.created_by = i.uid');
		$query ->fields('i',array('outlet'));
		$query ->fields('p',array('gmid','created_date','point','invoice_id','amount'));
		$query ->fields('m',array('member_level'));
		$query ->orderBy('p.created_date','desc');
		$result = $query ->execute();

		$outlet_arr = atoz_get_outlet();
		$member_level_arr = atoz_member_level();

	    foreach ($result as $row) {

	    	if($row->member_level == ''){
	    		$member_level = '';
	    	}else{
				$member_level = $member_level_arr[$row->member_level];
	    	}

	        $point = array();
	        $point[] = myst_clean_for_csv(date('d-M-y',strtotime($row->created_date)));
			$point[] = myst_clean_for_csv($row->gmid);
			$point[] = myst_clean_for_csv($row->invoice_id);
			$point[] = myst_clean_for_csv($row->amount);
			$point[] = myst_clean_for_csv($row->point);
			$point[] = myst_clean_for_csv($outlet_arr[$row->outlet]);
			$point[] = myst_clean_for_csv($member_level);

	        echo implode (",", $point) . "\n";
	    }
	    exit(0);
	}
?>