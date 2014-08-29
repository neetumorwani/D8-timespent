(function ($) {
  Drupal.behaviors.time_spent = {
  attach: function(context) {
    // Detect if is in an iframe, like overlay module.
    var isInIFrame = (window.location != window.parent.location) ? true : false;
    // Initialize the timer.
    var timer = setInterval( time_spent_ajax, (Drupal.settings.time_spent.timer * 1000));
    // Limit how long the timer will run (in minutes).
    setTimeout(function() {clearInterval(timer);}, Drupal.settings.time_spent.limit * 1000 * 60);
    window.parent.enabled = true;
    if(!isInIFrame){
    }
    else{
      // If a page is loaded into a overlay iframe
      // cancel the timespent from the page under overlay.
      window.parent.enabled = false;
    }    
    $('#overlay-close').click(function() {
      window.parent.enabled = true;
    });

    /**
     * Send pings to the backend for to update time spent.
     */
    function time_spent_ajax() {
      if(isInIFrame || window.parent.enabled){
        $.ajax({
          type: 'get',
          url: Drupal.settings.basePath + 'js/time_spent/ajax/' + Drupal.settings.time_spent.nid + '/' + Drupal.settings.time_spent.sectoken, 
          dataType: 'json', 
          data: 'js=1' 
        });
      }
    }
   }
};
})(jQuery);