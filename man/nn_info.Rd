% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/nnInfo.R
\name{nn_info}
\alias{nn_info}
\title{nn_info - obtain basic information about an iQuest neural network model.}
\usage{
nn_info(model_name)
}
\arguments{
\item{model_name}{Name of the iQuest *.enn file that defines the neural network model.}
}
\value{
List containing number of input, output, and hidden neurons, respectively.
}
\description{
nn_info - obtain basic information about an iQuest neural network model.
}
\examples{
filename <- file.path(system.file("extdata/WI_StreamTemp.enn", package="nnlib") )
nn_list <- nn_info( filename )  ## responds with a printout of key neural network features.
cat("There should be 17 input nodes. WI_StreamTemp.enn specifies", nn_list$num_input, "nodes.")
cat("There should be 1 output node. WI_StreamTemp.enn specifies", nn_list$num_output, "node.")
cat("There should be 3 hidden nodes. WI_StreamTemp.enn specifies", nn_list$num_hidden, "nodes.")
}

