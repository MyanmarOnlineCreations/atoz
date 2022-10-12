<?php
date_default_timezone_set("Asia/Yangon");
function atoz_admin_init () {
     $server = $_SERVER["SERVER_NAME"];

	  drupal_add_css(drupal_get_path('module', 'atoz') . '/css/atoz_admin.css', array("group"=> CSS_THEME));
	  drupal_add_js(drupal_get_path('module', 'atoz') . '/js/atoz_admin.js');
	  drupal_add_js('https://ajax.googleapis.com/ajax/libs/myanmar-tools/1.2.1/zawgyi_detector.min.js');
	  drupal_add_js('https://ajax.googleapis.com/ajax/libs/myanmar-tools/1.2.1/zawgyi_converter.min.js');
}

function atoz_navbar ($parameter1) {

	$buttons = array (
		"atoz" => array ("url"=>"/atoz", "label"=>t('Home')),
	);

    global $user;
  	if (in_array('super admin', $user->roles)) {
	    $buttons["account_verification"] =  array ("url"=>"/atoz/account_verification", "label"=>t('Account Verification'));
	    $buttons["member"] =  array ("url"=>"/atoz/member", "label"=>t('Member'));
	    $buttons["redemption"] =  array ("url"=>"/atoz/redemption", "label"=>t('Redemption List'));
	    $buttons["memberreport"] =  array ("url"=>"/atoz/memberreport", "label"=>t('Member Report'));
	    $buttons["pointreport"] =  array ("url"=>"/atoz/pointreport", "label"=>t('Point Management Report'));
	    $buttons["redemptionreport"] =  array ("url"=>"/atoz/redemptionreport", "label"=>t('Redemption Report'));
	    $buttons["redemption_history"] =  array ("url"=>"/atoz/redemption_history", "label"=>t('Redemption History'));	    
	    $buttons["memberlayer_rules"] =  array ("url"=>"/atoz/memberlayer_rules", "label"=>t('Rules'));
	    $buttons["pending"] =  array ("url"=>"/atoz/pending", "label"=>t('Pending'));
	    $buttons["staff"] =  array ("url"=>"/atoz/staff", "label"=>t('Staff'));
	    $buttons["promotions"] =  array ("url"=>"/atoz/promotions", "label"=>t('Promotions'));
  	    $buttons["outlet"] =  array ("url"=>"/atoz/outlet", "label"=>t('Outlets'));
	    $buttons["birthday"] =  array ("url"=>"/atoz/birthday", "label"=>t('Birthday'));
  	    
    }else if(in_array('atoz admin', $user->roles)){
		
    }else if(in_array('staff admin',$user->roles)){
    	$buttons["account_verification"] =  array ("url"=>"/atoz/account_verification", "label"=>t('Account Verification'));
	    $buttons["member"] =  array ("url"=>"/atoz/member", "label"=>t('Member'));    	
    	$buttons["memberreport"] =  array ("url"=>"/atoz/memberreport", "label"=>t('Member Report'));
	    $buttons["pointreport"] =  array ("url"=>"/atoz/pointreport", "label"=>t('Point Management Report'));
	    $buttons["redemptionreport"] =  array ("url"=>"/atoz/redemptionreport", "label"=>t('Redemption Report'));
	    $buttons["redemption_history"] =  array ("url"=>"/atoz/redemption_history", "label"=>t('Redemption History'));
	    $buttons["pending"] =  array ("url"=>"/atoz/pending", "label"=>t('Pending'));
	    $buttons["staff"] =  array ("url"=>"/atoz/staff", "label"=>t('Staff'));
	    $buttons["promotions"] =  array ("url"=>"/atoz/promotions", "label"=>t('Promotions'));
  	    $buttons["outlet"] =  array ("url"=>"/atoz/outlet", "label"=>t('Outlets'));	    
	    $buttons["birthday"] =  array ("url"=>"/atoz/birthday", "label"=>t('Birthday'));	    
    }else if(in_array('branch', $user->roles)){
    	$buttons["account_verification"] =  array ("url"=>"/atoz/account_verification", "label"=>t('Account Verification'));
	    $buttons["member"] =  array ("url"=>"/atoz/member", "label"=>t('Member')); 
	    $buttons["pending"] =  array ("url"=>"/atoz/pending", "label"=>t('Pending'));
	    
	    $buttons["birthday"] =  array ("url"=>"/atoz/birthday", "label"=>t('Birthday'));	    

    }
	$output = '
	<div id="primary_tabs" class="tabs ">
		<h2 class="element-invisible">Primary tabs</h2>
		<ul class="tabs primary">';

	foreach ($buttons as $category=>$info) {
		if (($category == $parameter1) || ($category == "atoz" && !$parameter1)) {
			$activeClass = "active";

			drupal_set_title ($info["label"]);
		}
		else {
			$activeClass = "";
		}

		$output .= '
		<li class="'.$activeClass.'"><a class="'.$activeClass.'" href="'.$info["url"].'">'.$info["label"].'</a></li>' . "\n";
	}

	$output .= "</ul>
	</div>";

  return $output;
}

function atoz ($parameter1, $parameter2, $parameter3, $parameter4) {

    // $output = '<a href="/user/logout"><div class="admin-option" style="float: right">' . t('Logout') . "</div></a>";

    $currentURL = request_uri();

    $split_arr = split('?', $currentURL);

    $url = $split_arr[0];



    // $output = '<a href="/user/logout"><div class="admin-option" style="float: right">' . t('Logout') . "</div></a>";

    $output = '<div class="admin-option" style="float: right"><a href="'.$url.'?language=en"> EN | </a> <a href="'.$url.'?language=my"> ျမန္မာ</a><br><a href="/user/logout">' . t('Logout') . '</a> </div>';

	$output .= atoz_navbar ($parameter1);


    if ($parameter1) {
        $functionName = "atoz_admin_" . $parameter1;

        if (function_exists($functionName)) {
            $output .= $functionName ($parameter2, $parameter3, $parameter4);
        }
    }
    else {
        $output .= "<h1>" . t('Welcome to ATOZ') . "</h1>";

        global $user;

        if (in_array('website admin', $user->roles)) {
            $output .= '<div class="admin-option"><a href="/admin/people/">Manage users</a></div>';
        }
    }

	return $output;
}

/*function atoz_admin_user ($parameter1 = null, $parameter2 = null, $parameter3 = null) {

    require_once ("atoz_admin_user.inc");

    return atoz_user ($parameter1, $parameter2, $parameter3);
}*/


function atoz_admin_pager ($tableName, $dataPerRow, $rowsPerPage, &$currentPage, &$offset, &$total) {

	$currentPage = 1;

	if (isset ($_GET["page"])) {
		$currentPage = $_GET["page"];
	}

	$offset = ($currentPage - 1) * $rowsPerPage;

	if (!$tableName) {
		// user must set the total
	}
	else {
		if (is_array ($tableName)) {
			$tableSpec = $tableName["tablename"];

			$conditionSpec = array ();

			if (isset ($tableName["condition"]))  {
				$conditionSpec = $tableName["condition"];
			}

			$likeSpec = array ();

			if (isset ($tableName["like"]))  {
				$likeSpec = $tableName["like"];
			}

			$query = null;

			if (isset ($tableName["group"])) {
				$groupSpec = $tableName["group"];
				$fieldSpec = $tableName["field"];

				$subquery = NULL;

				$alias = "";

				foreach ($tableSpec as $tName=>$tAlias) {
					$subquery = db_select ($tName, $tAlias);
					$alias = $tAlias;
					break;
				}

				if (isset ($tableName["join"])) {
					$joinSpec = $tableName["join"];

					foreach ($joinSpec as $tName=>$tSpec) {
						$subquery->join ($tName, $tSpec["alias"], $tSpec["condition"]);
					}
				}

				foreach ($conditionSpec as $tField=>$tCondition) {
				    if (is_array ($tCondition)) {
				        foreach ($tCondition as $value=>$expression) {
        					$subquery->condition ($tField, $value, $expression);
        				}
				    }
				    else {
    					$subquery->condition ($tField, $tCondition);
    				}
				}

				foreach ($likeSpec as $tField=>$tCondition) {
					$subquery->condition ($tField, $tCondition, "like");
				}

				foreach ($groupSpec as $tField) {
					$subquery->groupby ($tField);
				}

				foreach ($fieldSpec as $tField=>$tAlias) {
					$subquery->addField ($tAlias, $tField);
				}

				$query = db_select ($subquery, "s");
			}
			else {
				foreach ($tableSpec as $tName=>$tAlias) {
					$query = db_select ($tName, $tAlias);
					break;
				}

				foreach ($conditionSpec as $tField=>$tCondition) {
				    if (is_array ($tCondition)) {
				        foreach ($tCondition as $value=>$expression) {
        					$query->condition ($tField, $value, $expression);
        				}
				    }
				    else {
    					$query->condition ($tField, $tCondition);
    				}
				}

                if (count($likeSpec) > 1) $multipleLike = true;
                else $multipleLike = false;

                $dbOrLike = db_or ();

				foreach ($likeSpec as $lField=>$lCondition) {
				    if ($multipleLike) {
    					$dbOrLike->condition ($lField, $lCondition, "like");
    				}
    				else {
    					$query->condition ($lField, $lCondition, "like");
    				}
				}

				if ($multipleLike) {
				    $query->condition ($dbOrLike);
				}
			}

			$query->addExpression ("count(*)", "total");

			$result = $query->execute();

			$total = 0;

			foreach ($result as $row) {
				$total = $row->total;
				break;
			}
		}
		else {
			$query = "SELECT count(*) as total from {".$tableName."}";

			$result = db_query ($query);

			$total = 0;

			foreach ($result as $row) {
				$total = $row->total;
				break;
			}
		}
	}

	$numOfPages = round( (float) $total / $rowsPerPage + 0.5, 0);

	$output = "";

	$statusText = '';
	$searchText = '';

	if (isset ($_GET["search"])) {
		$search = $_GET["search"];
		$searchText = "&search=" . $search;
	}

	$prefix = "";

	foreach ($_GET as $parameter=>$value) {
		if ($parameter <> "q" && $parameter <> "page" && $parameter <> "search") {
			if (!$prefix) $prefix .= "?";
			else $prefix .= "&";

			$prefix .= $parameter . '=' . $value;
		}
	}

	if ($numOfPages > 1) {

		if ($prefix) {
			$prefix .= '&';
		}
		else {
			$prefix = '?';
		}

		$output = '
		<div class="atoz-admin-pager">';

		if ($currentPage > 1) {
			$output .= '<a href="'.$prefix.'page=' . ($currentPage - 1) .$searchText. '">&lt;</a> ';
		}

		for ($i = 1; $i <= $numOfPages; $i++) {
			if ($currentPage == $i) {
				$output .= '<span class="ot-selected">'. $i . " </span>";
			}
			else {
				$output .= '<a href="'.$prefix.'page=' . $i . $searchText. '">'. $i.' </a> ';
			}
		}

		if ($currentPage < $numOfPages) {
			$output .= '<a href="'.$prefix.'page=' . ($currentPage + 1) . $searchText. '">&gt;</a> ';
		}

		$output .= '
		</div>';
	}

	return $output;
}

function atoz_admin_account_verification ($parameter1 = null, $parameter2 = null, $parameter3 = null, $parameter4 = null, $parameter5 = null) {
    //echo "12345";
     require_once ("atoz_admin_account_verification.inc");

     return myatoz_account_verification ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}
function atoz_admin_member ($parameter1 = null, $parameter2 = null, $parameter3 = null, $parameter4 = null, $parameter5 = null) {
    //echo "12345";
     require_once ("atoz_admin_member.inc");

     return myatoz_member ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}
function atoz_admin_redemption ($parameter1 = null, $parameter2 = null, $parameter3 = null, $parameter4 = null, $parameter5 = null) {
    //echo "12345";
     require_once ("atoz_admin_redemption.inc");

     return myatoz_redemption ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}

function atoz_admin_redemption_history ($parameter1 = null, $parameter2 = null, $parameter3 = null, $parameter4 = null, $parameter5 = null) {
    //echo "12345";
     require_once ("atoz_admin_redemption_history.inc");

     return myatoz_redemption_history ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}

function atoz_admin_memberlayer_rules ($parameter1 = null, $parameter2 = null, $parameter3 = null, $parameter4 = null, $parameter5 = null) {
    //echo "12345";
     require_once ("atoz_admin_rules.inc");

     return myatoz_rules($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}
function atoz_admin_pending ($parameter1 = null, $parameter2 = null, $parameter3 = null, $parameter4 = null, $parameter5 = null) {
    //echo "12345";
     require_once ("atoz_admin_pending.inc");

     return myatoz_pending ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}
function atoz_admin_staff ($parameter1 = null, $parameter2 = null, $parameter3 = null, $parameter4 = null, $parameter5 = null) {
    //echo "12345";
     require_once ("atoz_admin_staff.inc");

     return myatoz_staff ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}
function atoz_admin_promotions ($parameter1 = null, $parameter2 = null, $parameter3 = null, $parameter4 = null, $parameter5 = null) {
    //echo "12345";
     require_once ("atoz_admin_promotions.inc");

     return myatoz_promotions ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}
function atoz_admin_birthday ($parameter1 = null, $parameter2 = null, $parameter3 = null, $parameter4 = null, $parameter5 = null) {
    //echo "12345";
     require_once ("atoz_admin_birthday.inc");

     return myatoz_birthday ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}

function atoz_admin_memberreport($parameter1 = null ,$parameter2 = null,  $parameter3 = null, $parameter4 = null, $parameter5 = null){

	require_once ("atoz_admin_member_report.inc");

    return myatoz_member_report ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}

function atoz_admin_pointreport ($parameter1 = null, $parameter2 = null, $parameter3 = null, $parameter4 = null, $parameter5 = null) {
    // echo "12345";
    require_once ("atoz_admin_point_report.inc");

    return myatoz_point_report ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}

function atoz_admin_redemptionreport ($parameter1 = null, $parameter2 = null, $parameter3 = null, $parameter4 = null, $parameter5 = null) {
    // echo "12345";
    require_once ("atoz_admin_redemption_report.inc");

    return myatoz_redemption_report ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5);
}

function atoz_admin_outlet($parameter1 = null, $parameter2 = null){
	require_once("atoz_admin_outlets.inc");
	require_once("atoz_util.inc");
	
	return myatoz_outlet($parameter1, $parameter2);
}


function atoz_admin_create_table ($tableID, $header, $data, $styleArray = null, $dataPerRow = 1, $groupByColumn = 0) {

	$style = "my-odd";

	$output = '
	<table id="' . $tableID. '" class="atoz-admin-table">';

	$output .= '
		<tr>';

	foreach ($header as $cell) {
		$columnClass = "";
		$columnAttributes = "";

		if (strpos ($cell, "[+]") !== FALSE) {
			$columnClass = 'st-column-min-max';
		}

		$image = "";
		if (strpos($cell, "atoz-current-sort") !== FALSE) {
			$columnClass .= " st-current-sort-cell";
		}

		$output .= '
			<th class="'.$columnClass. '" ' . $columnAttributes.'>' . $cell . '</th>';
	}

	$output .= '
		</tr>';

	$lastCell = "";
	$commonCell = false;

	$rowIndex = 0;

	$style2 = "atoz-admin-table-cell-filled";

	$activeSet = false;

	foreach ($data as $row) {
		$cellIndex = 1;

		$prefix = "";

		if ($dataPerRow > 1) {
			if ($rowIndex % $dataPerRow == 0) {
				$prefix = '
					<tr>';
			}
		}
		else {
			$prefix = '
				<tr>';
		}

		$rowOutput = "";

		if (isset ($row["class"])) {
			if ($prefix) {
				$prefix = '<tr class="' . $row["class"].'">';
			}

			unset ($row["class"]);
		}

		foreach ($row as $index=>$cell) {

			$showCell = true;

			if ($cellIndex == $groupByColumn) {
				if ($lastCell == $cell) {
					$commonCell = true;
					$showCell = false;
					$style2 = "";
				}
				else {
					$activeSet = false;
					$commonCell = false;
					$lastCell = $cell;
					$style2 = "atoz-admin-table-cell-filled";

					if ($rowIndex > 0) {
						if ($style == "atoz-odd") {
							$style = "atoz-even";
						}
						else {
							$style = "atoz-odd";
						}
					}
				}
			}

			$style3 = "";

			if ($styleArray && isset ($styleArray[$cellIndex-1]) && $styleArray[$cellIndex-1]) {
				$style3 = "atoz-cell-" . $styleArray[$cellIndex-1];
			}

			$styleX = "";

			// have to check whether we need to add an extra heading in case the first row was in-active
			if (strpos ($prefix, "atoz-active") > 0) {
				if ($commonCell && !$activeSet) {
					$showCell = true;
					$styleX = "atoz-extra-heading";
				}
				$activeSet = true;
			}

			if ($showCell) {
				$rowOutput .= '
				<td class="'. $style. ' ' . $style2 .' ' . $style3 .'"><span class="'.$styleX.'">'. $cell . '</span></td>';
			}
			else {
				$rowOutput .= '
				<td class="'. $style. ' ' . $style2 . '"></td>';
			}

			$cellIndex++;
		}

		$output .= $prefix . $rowOutput;

		$rowIndex++;

		$terminate = true;

		if ($dataPerRow > 1) {
			if ($rowIndex % $dataPerRow != 0) {
				$terminate = false;
			}
		}

		if ($terminate) {
			$output .= '
				</tr>';

			if ($groupByColumn == 0) {
				if ($style == "atoz-odd") {
					$style = "atoz-even";
				}
				else {
					$style = "atoz-odd";
				}
			}
		}
	}

	if ($dataPerRow > 1) {
		$terminate = false;
		while ($rowIndex % $dataPerRow > 0) {
			$output .= '<td class="'. $style. '">&nbsp;</td>';
			$rowIndex++;
			$terminate = true;
		}

		if ($terminate) {
				$output .= '
					</tr>';
		}

	}

	$output .= '
	</table>
	';

	return $output;
}

function atoz_generateHeader ($headerFields, $sortOptions) {
	$header = array ();

	$currentField = "";

	if (isset ($_GET["sortby"])) {
		$currentField = $_GET["sortby"];
	}

	$currentSort = "";

	if (isset ($_GET["order"])) {
		$currentSort = $_GET["order"];
	}

	$defaultField = "";

	if (isset ($sortOptions["default"])) {
		$defaultField = $sortOptions["default"];
	}

	$prefix = "";

	foreach ($_GET as $parameter=>$value) {
		if ($parameter <> "q" && $parameter <> "sortby" && $parameter <> "order") {
			if (!$prefix) $prefix .= "?";
			else $prefix .= "&";

			$prefix .= $parameter . '=' . $value;
		}
	}

	foreach ($headerFields as $headerTitle=>$sortField) {
		if ($sortField) {

			$order = "asc";

			$highlight = false;

			if ($currentField == $sortField) {
				if ($currentSort == "asc") {
					$order = "desc";
				}
				else if ($currentSort == "desc") {
					$order = "asc";
				}

				$highlight = true;

			}
			else {
				if (!$currentField && $defaultField == $sortField) {
					// swop because we are already going be use the sort order
					$order = $sortOptions[$sortField];

					if ($order == "asc") {
						$order = "desc";
					}
					else if ($order == "desc") {
						$order = "asc";
					}

					$highlight = true;
				}
				else {
					if (isset($sortOptions[$sortField])) {
						$order = $sortOptions[$sortField];
					}
				}
			}

			if ($prefix) $prefix2 = '&';
			else $prefix2 = '?';

			$link = '<a href="'.$prefix.$prefix2.'sortby='.$sortField.'&order='.$order.'">'.$headerTitle.'</a>';

			if ($highlight) {

				if ($order == "asc") {
					$image = '
						<img src="/themes/seven/images/arrow-desc.png" alt="sort descending" title="sort descending" height="13" width="13">';
				}
				else {
					$image = '
						<img src="/themes/seven/images/arrow-asc.png" alt="sort ascending" title="sort ascending" height="13" width="13">';
				}

				$header[] = '<span class="atoz-current-sort">' . $link . $image . '</span>';
			}
			else {
				$header[] = $link;
			}
		}
		else {
			$header[] = $headerTitle;
		}
	}

	return $header;
}

function atoz_checksort ($sortOptions, &$query) {

	$sortBy = "";
	$order = "ASC";

	if (isset ($_GET["sortby"])) {
		$sortBy = $_GET["sortby"];
	}
	else if (isset ($sortOptions["default"])) {
		$sortBy = $sortOptions["default"];
	}

	if ($sortBy && isset($sortOptions[$sortBy])) {

		if (isset ($_GET["order"])) {
			$order = $_GET["order"];
		}
		else {
			$order = $sortOptions[$sortBy];
		}

		$query->orderby ($sortBy, $order);
	}
}

function atoz_admin_page_offset ($rowsPerPage) {
    $currentPage = 1;

    if (isset ($_GET["page"])) {
        $currentPage = $_GET["page"];
    }

    $offset = ($currentPage - 1) * $rowsPerPage;

    return $offset;
}


