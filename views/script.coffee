#///////////////////////////////////////////////////////////////////////////////
# Raspi GPIO Coffeescript
#///////////////////////////////////////////////////////////////////////////////

new_pin = (p) ->
  txt = p
  txt = pins[p-1].name.split(' ')[0] unless pins[p-1].memory_index 
  pins[p-1].cell = $("<td/ class=\"pin#{p}\">")
    .data('pin_no', p)
    .text(txt)
    .click ->
      toggle_pin $(this).data('pin_no')

toggle_pin = (p) ->
  console.log p + ' / ' + pins[p-1].value
  if pins[p-1].value
    $.post "/write/#{p}/0"
  else
    $.post "/write/#{p}/1"


new_pinout = (p) ->
  pins[p-1].io = pin = $("<td/ id=\"pin#{p}-io\">")
  if pins[p-1].memory_index == 0
    pin.css 'opacity', 0
  pin.text 'X'
  return pin


new_pin_row = (p) ->
  row = pins[p-1].row = $('.pin-template').clone()
  row.removeClass 'pin-template'
  row.find('.pin').text p
  row.find('.name').text pins[p-1].name
  row.find('.gpio').text pins[p-1].gpio_no
  pins[p-1].rowstate = row.find('.state')
  pins[p-1].rowvalue = row.find('.value')
  row.data 'pin', pins[p-1]
  row.hover \
    ( -> $(this).data('pin').cell.addClass 'underlined'), \
    ( -> $(this).data('pin').cell.removeClass 'underlined')
  pins[p-1].rowstate.data 'pin_no', p
  pins[p-1].rowstate.find('.input-pill, .output-pill').click ->
    pin_no = $(this).parent().parent().data 'pin_no'
    console.log pin_no
    $.post "/set/#{pin_no}/#{$(this).attr('val')}"
  return row

$ ->
  for i in [1..13]
    $('#chip .odds').append   new_pin(2*i - 1)
    $('#odd-readings').append new_pinout(2*i - 1)
    $('#pin-table').append new_pin_row(2*i - 1) if pins[2*i - 2].memory_index
    $('#chip .evens').append new_pin(2*i)
    $('#even-readings').append new_pinout(2*i)
    $('#pin-table').append new_pin_row(2*i) if pins[2*i - 1].memory_index
  socket = new WebSocket "ws://#{document.domain}:4567"
  socket.onmessage = update_pins

update_pins = (mssg) ->
  pin_states = $.parseJSON mssg.data
  for i in [0..25]
    pins[i].state = pin_states[i][0]
    pins[i].value = pin_states[i][1]
    pin = pins[i]
    if pin.memory_index
      pin.rowstate.find('.active').removeClass 'active'
      if pin_states[i][0] == 1
        pin.cell.addClass 'output'
        pin.cell.removeClass 'input'
        pin.row.removeClass 'error'
        pin.row.addClass 'success'
        pin.rowstate.find('.output-pill').addClass 'active'
      else
        pin.cell.addClass 'input'
        pin.cell.removeClass 'output'
        pin.row.removeClass 'success'
        pin.row.addClass 'error'
        pin.rowstate.find('.input-pill').addClass 'active'
      if pin_states[i][1] == 1
        pin.io.css 'opacity', 1
      else
        pin.io.css 'opacity', 0
      pin.rowvalue.text pin_states[i][1]

pins = [
  {   # 1
    gpio_no : 'NA'
    name : '3.3v Power'
    memory_index : 0
  },
  {   # 2
    gpio_no : 'NA'
    name : '5v Power'
    memory_index : 0
  },
  {   # 3
    gpio_no : '8'
    name : 'SDA - Gpio 8'
    memory_index : 2
  },
  {   # 4
    gpio_no : 'NA'
    name : '5v Power'
    memory_index : 0
  },
  {   # 5
    gpio_no : '9'
    name : 'SCL - Gpio 9'
    memory_index : 3
  },
  {   # 6
    gpio_no : 'NA'
    name : '0v Ground'
    memory_index : 0
  },
  {   # 7
    gpio_no : '7'
    name : 'GPCLK - Gpio 7'
    memory_index : 4
  },
  {   # 8
    gpio_no : '15'
    name : 'TXD - Gpio 15'
    memory_index : 14
  },
  {   # 9
    gpio_no : 'NA'
    name : '0v Ground'
    memory_index : 0
  },
  {   # 10
    gpio_no : '16'
    name : 'RXD - Gpio 16'
    memory_index : 15
  },
  {   # 11
    gpio_no : '0'
    name : 'Gpio 0'
    memory_index : 17
  },
  {   # 12
    gpio_no : '1'
    name : 'PCM_CLK - Gpio 1'
    memory_index : 18
  },
  {   # 13
    gpio_no : '2'
    name : 'Gpio 2'
    memory_index : 27
  },
  {   # 14
    gpio_no : 'NA'
    name : '0v Ground'
    memory_index : 0
  },
  {   # 15
    gpio_no : '3'
    name : 'Gpio 3'
    memory_index : 22
  },
  {   # 16
    gpio_no : '4'
    name : 'Gpio 4'
    memory_index : 23
  },
  {   # 17
    gpio_no : 'NA'
    name : '3.3v Power'
    memory_index : 0
  },
  {   # 18
    gpio_no : '5'
    name : 'Gpio 5'
    memory_index : 24
  },
  {   # 19
    gpio_no : '12'
    name : 'MOSI - Gpio 12'
    memory_index : 10
  },
  {   # 20
    gpio_no : 'NA'
    name : '0v Ground'
    memory_index : 0
  },
  {   # 21
    gpio_no : '13'
    name : 'MISO - Gpio 13'
    memory_index : 9
  },
  {   # 22
    gpio_no : '6'
    name : 'Gpio 6'
    memory_index : 25
  },
  {   # 23
    gpio_no : '14'
    name : 'SCLK - Gpio 14'
    memory_index : 11
  },
  {   # 24
    gpio_no : '10'
    name : 'CE0 - Gpio 10'
    memory_index : 8
  },
  {   # 25
    gpio_no : 'NA'
    name : '0v Ground'
    memory_index : 0
  },
  {   # 26
    gpio_no : '11'
    name : 'CE1 - Gpio 11'
    memory_index : 7
  }
]
