#!/usr/bin/env ruby
require 'serialport'

ENV['RAILS_ENV'] ||= 'development'
require File.join(File.dirname(__FILE__), *%w[.. .. config environment])

@serial = SerialPort.new '/dev/ttyUSB0'
@serial.baud = 9600
@serial.flow_control = SerialPort::NONE
@serial.parity = SerialPort::NONE
@serial.data_bits = 8
@serial.stop_bits = 1
@serial.read_timeout = 2000

Signal.trap('TERM') do 
end
      
def getok
  @serial.puts('*')
  if (@serial.gets.to_s) =~ /^[*]/
    return true
  else
    return false
  end
end

def getcmd(command = nil, result)
  @serial.puts(command)
  if (result.replace @serial.gets.to_s) =~ /^#{command}/
    return true
  else
    return false
  end
end

def create_device(uid, size, dump)
  begin
    Tag.create(uid: uid, size: size, dump: dump)
  rescue
  end
end

loop do
  if getok
    if getcmd('-', result = String.new)
      # line = Time.now.strftime '%Y-%m-%d, %H:%M:%S %z: ' + result
      p 'ok'
      create_device result.scan(/\d+/).join.to_i, 0, 0
    end
  end
  @serial.break 1
end

