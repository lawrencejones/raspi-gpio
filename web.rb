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
  ffi_lib File.join(File.expand_path('build/gpio'), 'gpio')
  attach_function :init_gpio_access, [], :pointer
  attach_function :init_chip, [], :pointer
  attach_function :chip_index_to_mem, [:int], :int
  attach_function :malloc_pin, [:int], :pointer
  attach_function :get_pin_status, [:int], :pointer
  attach_function :set_pin_value, [:int, :int], :void
  attach_function :set_pin_state, [:int, :int], :void
  attach_function :get_chip, [], :pointer
  attach_function :update_all_pins, [], :pointer
end

class PinStruct < FFI::Struct
  layout :chipIndex, :int, 
    :memIndex, :int,
    :state, :int,
    :value, :int

  def get_json
    [ self[:state], self[:value] ]
  end
end

class Chip < FFI::Struct
  layout :pins, :pointer

  def pin(n)
    PinStruct.new self.p_pointers[n-1]
  end

  def p_pointers
    Gpio.update_all_pins().get_array_of_pointer(0,26)
  end

  def get_pins
    self.p_pointers().map { |p| PinStruct.new(p).get_json }
  end
end

Gpio.init_gpio_access()
Gpio.init_chip()

EM.run do

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
      Gpio.set_pin_value (params[:pin].to_i), (params[:value].to_i)
    end

    post '/set/:pin/:value' do
      Gpio.set_pin_state (params[:pin].to_i), (params[:value].to_i)
    end

    get '/status' do
      chip = Chip.new Gpio.get_chip()
      chip.get_pins.to_json
    end

  end

  EM::WebSocket.start(:host => '0.0.0.0', :port => 4567) do |ws|
    ws.onopen { |handshake|
      puts "New connection from #{handshake.origin}"
      EM.add_periodic_timer(0.5) {
        chip = Chip.new Gpio.get_chip()
        ws.send chip.get_pins.to_json
      }
    }
    ws.onclose {
      puts "Closed connection"
    }
  end

  Thin::Server.start App, '0.0.0.0', 3000

end
