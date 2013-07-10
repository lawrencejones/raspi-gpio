#///////////////////////////////////////////////////////////////////////////////
# Raspi GPIO Coffeescript
#///////////////////////////////////////////////////////////////////////////////

new_pin = (p) ->
  button = $("<td/ class=\"pin#{p}\">")
    .click ->
      $(this).toggleClass 'input'

new_pinout = (p) ->
  cell = $("<td/ id=\"pin#{p}-io\">")
  cell.text 'X'
  return cell

$ ->
  for i in [1..13]
    $('#chip .odds').append   new_pin(2*i - 1)
    $('#odd-readings').append new_pinout(2*i - 1)
    $('#chip .evens').append new_pin(2*i)
    $('#even-readings').append new_pinout(2*i - 1)
  socket = new WebSocket "ws://#{document.domain}:4567"
  socket.onmessage = update_pins


update_pins = (mssg) ->
  console.log mssg
  for i in [1..26]
    cell = $("#chip .pin#{i}")
    cell.text(i)

pins = {
  1 : {
    gpio_no : 'NA'
    name : '3.3v Power'
    memory_index : 0
  }
  2 : {
    gpio_no : 'NA'
    name : '5v Power'
    memory_index : 0
  }
  3 : {
    gpio_no : '8'
    name : 'SDA - Gpio 8'
    memory_index : 2
  }
  4 : {
    gpio_no : 'NA'
    name : '5v Power'
    memory_index : 0
  }
  5 : {
    gpio_no : '9'
    name : 'SCL - Gpio 9'
    memory_index : 3
  }
  6 : {
    gpio_no : 'NA'
    name : '0v Ground'
    memory_index : 0
  }
  7 : {
    gpio_no : '7'
    name : 'GPCLK - Gpio 7'
    memory_index : 4
  }
  8 : {
    gpio_no : '15'
    name : 'TXD - Gpio 15'
    memory_index : 14
  }
  9 : {
    gpio_no : 'NA'
    name : '0v Ground'
    memory_index : 0
  }
  10 : {
    gpio_no : '16'
    name : 'RXD - Gpio 16'
    memory_index : 15
  }
  11 : {
    gpio_no : '0'
    name : 'Gpio 0'
    memory_index : 17
  }
  12 : {
    gpio_no : '1'
    name : 'PCM_CLK - Gpio 1'
    memory_index : 18
  }
  13 : {
    gpio_no : '2'
    name : 'Gpio 2'
    memory_index : 27
  }
  14 : {
    gpio_no : 'NA'
    name : '0v Ground'
    memory_index : 0
  }
  15 : {
    gpio_no : '3'
    name : 'Gpio 3'
    memory_index : 22
  }
  16 : {
    gpio_no : '4'
    name : 'Gpio 4'
    memory_index : 23
  }
  17 : {
    gpio_no : 'NA'
    name : '3.3v Power'
    memory_index : 0
  }
  18 : {
    gpio_no : '5'
    name : 'Gpio 5'
    memory_index : 24
  }
  19 : {
    gpio_no : '12'
    name : 'MOSI - Gpio 12'
    memory_index : 10
  }
  20 : {
    gpio_no : 'NA'
    name : '0v Ground'
    memory_index : 0
  }
  21 : {
    gpio_no : '13'
    name : 'MISO - Gpio 13'
    memory_index : 9
  }
  22 : {
    gpio_no : '6'
    name : 'Gpio 6'
    memory_index : 25
  }
  23 : {
    gpio_no : '14'
    name : 'SCLK - Gpio 14'
    memory_index : 11
  }
  24 : {
    gpio_no : '10'
    name : 'CE0 - Gpio 10'
    memory_index : 8
  }
  25 : {
    gpio_no : 'NA'
    name : '0v Ground'
    memory_index : 0
  }
  26 : {
    gpio_no : '11'
    name : 'CE1 - Gpio 11'
    memory_index : 7
  }
}
