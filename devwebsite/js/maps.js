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
