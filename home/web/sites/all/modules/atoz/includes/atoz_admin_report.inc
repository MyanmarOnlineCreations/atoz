<?php
function myatoz_report ($parameter1, $parameter2, $parameter3, $parameter4, $parameter5=null) {

    $output = '';
    
    $output .= atoz_report ();
    return $output;
}

function atoz_report()
{
	//drupal_add_css(drupal_get_path('module', 'atoz') . '/css/hti_admin_intake.css', array("group"=> CSS_THEME));
    drupal_add_js(drupal_get_path('module', 'atoz') . '/js/chart.min.js');
    $output = '<table class="atoz_admin_report">
        <tr><th style="text-align:center">SN</th><th style="text-align:center">Month</th><th style="text-align:center">Total Wards</th><th style="text-align:center">Total Unit Used</th><th style="text-align:center">Unit Cost</th><th style="text-align:center">Total Cost</th></tr>
        <tr><td style="text-align:right">1</td><td style="text-align:center">Jan-21</td><td>5</td><td>2343546</td><td>35</td><td style="text-align:right">82,024,110</td></tr>
        <tr><td style="text-align:right">2</td><td style="text-align:center">Feb-21</td><td>5</td><td>3446476</td><td>35</td><td style="text-align:right">120,626,660</td></tr>
        <tr><td style="text-align:right">3</td><td style="text-align:center">Mar-21</td><td>5</td><td>453426</td><td>35</td><td style="text-align:right">15,869,910</td></tr>
        <tr><td style="text-align:right">4</td><td style="text-align:center">Prepaid</td><td></td><td></td><td></td><td style="text-align:right">500,000</td></tr>
        <tr><td style="text-align:right">5</td><td style="text-align:center">Debts</td><td></td><td></td><td></td><td style="text-align:right">34,025,250</td></tr>
        <tr><td colspan="5" style="text-align:center">Total</td><td style="text-align:right">254,644,500</td></tr>
    </table>
    <canvas id="myChart" width="400" height="400"></canvas>    
<script>
var ctx = document.getElementById("myChart").getContext("2d");
var labels = ["19 20 ", "19 20", "19 20", "19 20", "19 20", "19 20","19 20" ,"20 21","20 21","20 21","20 21","20 21"];
var myChart = new Chart(ctx, {
    type: "bar",
    data: {
        labels: labels,
        datasets: [
            {
              label: "Last Year",
              data: [12, 19, 3, 5, 2, 3 ,6 , 10,15,23,22,12],
              backgroundColor: "blue",
            },
            {
              label: "This Year",
              data: [10, 22, 13, 15, 12, 10,16 , 12,13,24,23,16],
              backgroundColor: "green",
            },
          ]
    },
    options: {
        responsive: true,
        plugins: {
          legend: {
            position: "top",
          },
          
        }
      }
});
</script>';
return $output;
}