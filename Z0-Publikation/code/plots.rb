#!/usr/bin/env ruby

require '../code/diagram.rb'
require 'fileutils'
cmd_opts = { :file_extension => "pdf",
             :output_dir => ".",
             :glob => "*.root"
           }


files = {}
Dir.glob(cmd_opts[:glob]) do |aFileName|
  aTFile = TFile.new aFileName
  unless aTFile.IsZombie
    h = {aTFile.GetName => aTFile}
    files.merge! h
  end
end

raise "No valid TFiles found!" if files.size == 0

Diagram::setup_style

# files.each do |aFileName,aTFile|
#   folder = File.expand_path(cmd_opts[:output_dir]+"/"+File.basename(aFileName, ".root"))
#   FileUtils.mkdir_p folder
#   Diagram::create_all_histograms [aTFile], folder, cmd_opts[:file_extension], {:size => [400,400],:logy => true, :normalize => false, :grid => true}
# end

folder = File.expand_path cmd_opts[:output_dir]+"/extra"

special_plot = [
  HistStruct.new(files['hadrons.root'],'N_Cluster','MC sample hadrons;number of clusters;counts'),
  HistStruct.new(files['muons.root'],'N_Cluster','MC sample muons'),
]
Diagram::create_from_stringstruct special_plot, folder+"/N_Cluster.#{cmd_opts[:file_extension]}", {:size => [400,400],:logy => true, :normalize => false, :grid => false, :legend => [0.4, 0.75, 0.8, 0.9]}


special_plot = [
  HistStruct.new(files['hadrons.root'],'E_vis','MC sample hadrons;E_{vis}/\sqrt{s};counts'),
  HistStruct.new(files['muons.root'],'E_vis','MC sample muons'),
]
Diagram::create_from_stringstruct special_plot, folder+"/E_vis.#{cmd_opts[:file_extension]}", {:size => [400,400],:logy => false, :normalize => false, :grid => false, :legend => [0.2, 0.75, 0.6, 0.9], :title_offset_y => 1.5}