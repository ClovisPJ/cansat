$(function () {
      $('#humidity').highcharts({
          chart: {
              type: 'spline',
              events: {
                  load: function () {
                    var series = this.series[0];
                    setInterval(function () {
                      var xhttp = new XMLHttpRequest();
                      xhttp.onreadystatechange = function() {
                        if (xhttp.readyState == 4 && xhttp.status == 200) {
                          var data = xhttp.responseText;
                          console.log(data);
                          series.addPoint(+data, true, true);
                        }
                      }
                      xhttp.open("GET", "tmp", true);
                      xhttp.send();
                    }, 1000);
                  }
              }
          },
          title: {
              text: ''
          },
          xAxis: {
            title: {
              text: 'Time (seconds)'
            }
          },
          yAxis: {
              title: {
                  text: 'Humidity'
              },
              plotLines: [{
                  value: 0,
                  width: 1,
                  color: '#808080'
              }]
          },
          tooltip: {
              valueSuffix: '%'
          },
          plotOptions: {
              series: {
                animation: {
                  duration: 0
                }
              }
          },
          exporting: {
              enabled: false
          },
          series: [{
              showInLegend: false,
              data: (function () {
                  var data = [], i;
                  for (i = -19; i <= 0; i += 1) {
                      data.push(1);
                  }
                  return data;
              }())
          }]
      });
});
