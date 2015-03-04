require_relative '../Eks'

RbMake.library(:EksCore, :Eks) do |l, p|
  l.exclude_sources = [ 'include/Math/Eigen/**/*' ]

  l.when(l.config.platform(:win))
   .when(p.config.debug) do |l|
    l.libraries << "DbgHelp"
  end

  l.export do |e|
    l.dependencies << :eigen
  end
end

RbMake.test(:EksCore, :EksTest) do |l, p|
  l.root += '/test'
end