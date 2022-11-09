/**********************************
Start/Emergency Stop Button
**********************************/
/*
$(".btn1").click(function ()
{
    if ($(this).text()=="Stop")
    {
        sendCommand()
        $(this).text("Start");
        sendCommand("Pod Start");
        $(this).removeClass('red');
        $(this).addClass('teal');
      }
    else
    {
        $(this).text("Stop");
        sendCommand("Pod Stop");
        $(this).removeClass('teal');
        $(this).addClass('red');
    }
});*/
/**********************************
Toggle Button
**********************************/
//this is triggered for every input with type=checkbox
$('input:checkbox').change( function(){
    if($(this).is(':checked')) {
        //add class checked (used for css styling)
        $(this).closest('div').addClass('checked').trigger('switch');
        
        //if this switch is a command switch then send the string stored in the on-command attribute of the switch
        if($(this).closest('div').attr('command')!=null) {
            sendCommand($(this).closest('div').attr('on-command'));
        }

    } 
    else {
        $(this).closest('div').removeClass('checked').trigger('switch');
        
        //if this switch is a command switch then send the string stored in the off-command attribute of the switch
        if($(this).closest('div').attr('command')!=null) {
            sendCommand($(this).closest('div').attr('off-command'));
        }
    }
});
/**
 * Event listener for the functional control toggle switch 
 */
$('.controls .switch input#controls-toggle').change(function() {
   $('.controls .collapsible').collapsible('open', 0);
});

/**
 * Event listener for command buttons 
 */
$('button[command=true').click(function() {
   sendCommand($(this).attr('commandByte'));
});

//append 4 bars to each battery
$('.battery').each(function(i) {
    for(var x=1; x<=4; x++){
        $(this).append('<div class="bar">');
    }
});


