# encoding: UTF-8
#!/bin/ruby
# Simple little web API to expose the gpio C library.
# Should serve live state information about all the pins.

require 'rubygems'
require 'ffi'
require 'em-websocket'
require 'coffee-script'
require 'haml'
require 'thin'
require 'sinatra/base'
require 'json'

# system "make"

module Gpio extend FFI::Library
  ffi_lib File.join(File.expand_path('bin'), 'gpio')
  attach_function :initialiseGpioAccess, [], :pointer
  attach_function :initialiseChip, [], :pointer
  attach_function :chipIndexToMem, [:int], :int
  attach_function :mallocPin, [:int], :pointer
  attach_function :pinStatus, [:int], :pointer
  attach_function :setPin, [:int, :int], :void
  attach_function :getChip, [], :pointer
  attach_function :updateAllPins, [], :pointer
end

class PinStruct < FFI::Struct
  layout :chipIndex, :int, 
    :memIndex, :int,
    :state, :int,
    :value, :int

  def get_json
=begin   # Potential object literal approach
    { 
      :chipIndex => self[:chipIndex], 
      :memIndex => self[:memIndex],
      :state => self[:state], 
      :value => self[:value]
    }.to_json
=end
    [ self[:state], self[:value] ]
  end
end

class Chip < FFI::Struct
  layout :pins, :pointer

  def pin(n)
    PinStruct.new self.p_pointers[n-1]
  end

  def p_pointers
    Gpio.updateAllPins().get_array_of_pointer(0,26)
  end

  def get_pins
    self.p_pointers().map { |p| PinStruct.new(p).get_json }
  end
end

EM.run do

  Gpio.initialiseGpioAccess()
  Gpio.initialiseChip()

  # Define the app behaviour
  class App < Sinatra::Base

    # Define the get to root
    get '/' do
      haml :index    # render index view
    end

    # Defines script asset serving
    get '/script.js' do
      coffee(:script)
    end

    # Write to the pin
    post '/write/:pin/:value' do
      Gpio.setPin (params[:pin].to_i), (params[:value].to_i)
    end

    get '/status' do
      chip = Chip.new Gpio.getChip()
      chip.get_pins.to_json
    end

  end

  EM::WebSocket.start(:host => '0.0.0.0', :port => 4567) do |ws|
    ws.onopen { |handshake|
      puts "New connection from #{handshake.origin}"
      EM.add_periodic_timer(0.1) {
        chip = Chip.new Gpio.getChip()
        ws.send chip.get_pins.to_json
      }
    }
    ws.onclose {
      puts "Closed connection"
    }
  end

  Thin::Server.start App, '0.0.0.0', 3000

end
