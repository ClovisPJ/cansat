var data = [];

function parsedata(max) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (xhttp.readyState == 4 && xhttp.status == 200) {
        var raw = xhttp.responseText;
        raw = raw.split('\n');
        for (i = 0; i < max; i++) {
          data = +(raw[raw.length-1].split(','));
        }
      }
    }
    xhttp.open("GET", "tmp", true);
    xhttp.send();
}

$(function () {
      $('#humidity').highcharts({
          chart: {
              type: 'areaspline',
              events: {
                load: function () {
                    var series1 = this.series[0];
                    var series2 = this.series[1];
                    setInterval(function () {
                      parsedata(2);
                      series1.addPoint( data[0], false, true, true);
                      series2.addPoint( data[1], true, true, true);
                    }, 1000);
                }
              }
          },
          title: {
              text: null 
          },
          xAxis: {
            title: {
              text: 'Time (seconds)'
            }
          },
          yAxis: {
              title: {
                  text: 'Humidity (% saturation)'
              }
          },
          tooltip: {
              valueSuffix: '%',
              hideDelay: 50
          },
          plotOptions: {
              series: {
                animation: {
                  duration: 0
                }
              },
              areaspline: {
                  fillColor: {
                      linearGradient: {
                          x1: 0,
                          y1: 0,
                          x2: 0,
                          y2: 1 
                      },
                      stops: [
                          [0, 'rgba(45,116,255,0.7)'],
                          [1, 'rgba(0,0,0,0)']
                      ]
                  },
                  marker: {
                      radius: 2
                  },
                  lineWidth: 1,
                  states: {
                      hover: {
                          lineWidth: 1.4
                      }
                  }
              }

          },
          exporting: {
              enabled: false
          },
          series: [{
              type: 'areaspline',
              showInLegend: false,
              data: (function () {
                  var data = [], i;
                  for (i = 0; i <= 1000; i += 100) {
                      data.push(i);
                  }
                  return data;
              }())
          }, {
              type: 'areaspline',
              showInLegend: false,
              data: (function () {
                  var data = [], i;
                  for (i = 1000; i >= 0; i -= 100) {
                      data.push(i);
                  }
                  return data;
              }())

          }]
      });
});

$(function () {
    $('#co').highcharts({
        chart: {
          type: 'solidgauge',
          events: {
            load: function() {
              point = this.series[0].points[0];
              var i = 0;
              setInterval(function () {
                point.update(i);
                i++;
              }, 50);
            }
          }
        },
        title: null,
        pane: {
            center: ['50%', '85%'],
            size: '100%',
            startAngle: -90,
            endAngle: 90,
            background: {
                backgroundColor: (Highcharts.theme && Highcharts.theme.background2) || '#EEE',
                innerRadius: '60%',
                outerRadius: '100%',
                shape: 'arc'
            }
        },

        tooltip: {
            enabled: false
        },

        // the value axis
        yAxis: {
            stops: [
                [0.1, '#2DDDFF'],
                [0.5, '#2D74FF'],
                [0.9, '#4F2dFF']
            ],
            lineWidth: 0,
            minorTickInterval: null,
            tickPixelInterval: 400,
            tickWidth: 0,
            title: {
                y: -70
            },
            labels: {
                y: 16
            },
            min: 0,
            max: 200,
            title: {
                text: 'Speed'
            }
        },

        plotOptions: {
            solidgauge: {
                dataLabels: {
                    y: 5,
                    borderWidth: 0,
                    useHTML: true
                }
            }
        },
        credits: {
            enabled: false
        },

        series: [{
            name: 'Speed',
            data: [0],
            dataLabels: {
                format: '<div style="text-align:center"><span style="font-size:25px;color:' +
                    ((Highcharts.theme && Highcharts.theme.contrastTextColor) || 'black') + '">{y}</span><br/>' +
                       '<span style="font-size:12px;color:silver">km/h</span></div>'
            },
            tooltip: {
                valueSuffix: ' km/h'
            }
        }]
    });
});

function initialize() {
  var loc = new google.maps.LatLng(52.481572, -0.468927);
  var loc1 = new google.maps.LatLng(52.481997, -0.467972);
  var loc2 = new google.maps.LatLng(52.483101, -0.467285);
  var loc3 = new google.maps.LatLng(52.483441, -0.466432);
  var loc4 = new google.maps.LatLng(52.482701, -0.466141);
  var mapProp = {
    center: loc,
    zoom:16,
    mapTypeId:google.maps.MapTypeId.ROADMAP
  };
  var map=new google.maps.Map(document.getElementById("googleMap"),mapProp);

  var marker = new google.maps.Marker({ position: loc });
  marker.setMap(map);

  var myTrip=[loc, loc1, loc2, loc3, loc4];
  var flightPath = new google.maps.Polyline({
    path:myTrip,
    strokeColor:"#0000FF",
    strokeOpacity:0.8,
    strokeWeight:2
  });
  flightPath.setMap(map);
}
google.maps.event.addDomListener(window, 'load', initialize);
