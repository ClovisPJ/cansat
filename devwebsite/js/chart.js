$(function () {
    $(document).ready(function () {
        $('#container').highcharts({
            chart: {
                type: 'spline',
                animation: Highcharts.svg, // don't animate in old IE
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
                text: 'Temperature of the Can',
                x: -20 //center
            },
            xAxis: {
              title: {
                text: 'Time (seconds)'
              }
            },
            yAxis: {
                title: {
                    text: 'Temperature (°C)'
                },
                plotLines: [{
                    value: 0,
                    width: 1,
                    color: '#808080'
                }]
            },
            tooltip: {
                valueSuffix: '°C'
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
                name: 'Temp',
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
});
