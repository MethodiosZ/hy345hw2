all:	itinerary

itinerary:	itinerary.c
		gcc -pthread itinerary.c -o itinerary

clean:
	rm -f itinerary
	clear
