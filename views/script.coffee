#///////////////////////////////////////////////////////////////////////////////
# Raspi GPIO Coffeescript
#///////////////////////////////////////////////////////////////////////////////

new_pin = (p) ->
  button = $("<td/ class=\"pin#{p}\">")
    .click ->
      alert "Hi, I'm #{$(this).attr('class')}"

$ ->
  for i in [1..13]
    $('#chip .odds').append  new_pin(27 - 2*i)
    $('#chip .evens').append new_pin(26 - 2*i)