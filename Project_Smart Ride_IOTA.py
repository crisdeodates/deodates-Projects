
#-------------------------------------------------------------------------------#
#-- PROJECT Smart Ride                                                          #
#                                                                               #
#-- PROGRAM GOALS                                                               #
#-- Implement IOTA to Transportation paradigm                                   #
#                                                                               #
#-- Programmers: Cris Thomas, Jiss Joseph Thomas                                #
#-- References: IOTA                                                            #
#-------------------------------------------------------------------------------#

from iota import Iota
from iota import Address
import time
import datetime
# import RPi.GPIO as gpio

# # Set RPi GPIO
# gpio.setmode(gpio.BCM)
# gpio.setup(40, gpio.OUT)
# gpio.setup(38, gpio.OUT)
# gpio.setup(36, gpio.OUT)
# gpio.setup(32, gpio.OUT)

# # Relay interfacings
# relay_1_pin = 40
# relay_2_pin = 38
# relay_3_pin = 36
# relay_4_pin = 32

# # Function to check balance on the IOTA tangle. 
# def balance_check():

# 	print("Checking balance")
# 	balance_result = iota_obj.get_balances(address)
# 	balance = balance_result['balances']
# 	return (balance[0])

# # IOTA fullnode URL
# iotaNode = "https:#nodes.thetangle.org:443"

# # Creating an IOTA object
# iota_obj = Iota(iotaNode, "")

# # IOTA address
# # Create your own address using Trinity Wallet
# address = [Address(b'DESCRIBEYOURADDRESSHERE')]

# # Obtain the current balance 
# currentbalance = balance_check()
# prev_balance = currentbalance

# Variables
balance = 0
balcheckcount = 0
act_status = False

# Main loop
while True:
	
	# Check for balance every 5 seconds.
	if balcheckcount == 5:
		print("Checking balance for address ", address)
		# currentbalance = balance_check()
		if currentbalance > prev_balance:
			balance = balance + (currentbalance - prev_balance)
			prev_balance = currentbalance
		balcheckcount = 0

	# Manage the activation status
	if balance > 0:
		if act_status == False:
			print("Balance Received.... Activating.....")
			act_status=True
		balance = balance -1       
	else:
		if act_status == True:
			act_status=False

	# Interaction based in activation status
	if act_status == True:
		# gpio.output(40, gpio.HIGH)
		# gpio.output(38, gpio.HIGH)
		# gpio.output(36, gpio.HIGH)
		# gpio.output(32, gpio.HIGH)

		time.sleep(5)

		# gpio.output(40, gpio.LOW)
		# gpio.output(38, gpio.LOW)
		# gpio.output(36, gpio.LOW)
		# gpio.output(32, gpio.LOW)
 
		# Print action
		print("Action completed")

	# Increase counter
	balcheckcount = balcheckcount +1

	# Pause 1 sec.
	time.sleep(1)
