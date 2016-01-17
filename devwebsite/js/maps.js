function initialize() {
  var loc = new google.maps.LatLng(51.508742,-0.120850);
  var loc1 = new google.maps.LatLng(55.508742,-1.570850);
  var loc2 = new google.maps.LatLng(59.508742, 2.320850);
  var mapProp = {
    center: loc,
    zoom:6,
    mapTypeId:google.maps.MapTypeId.ROADMAP
  };
  var map=new google.maps.Map(document.getElementById("googleMap"),mapProp);

  var marker = new google.maps.Marker({ position: loc });
  marker.setMap(map);

  var myTrip=[loc, loc1, loc2];
  var flightPath = new google.maps.Polyline({
    path:myTrip,
    strokeColor:"#0000FF",
    strokeOpacity:0.8,
    strokeWeight:2
  });
  flightPath.setMap(map);
}
google.maps.event.addDomListener(window, 'load', initialize);
