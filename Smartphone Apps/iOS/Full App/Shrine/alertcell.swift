//
//  alertcell.swift
//  Shrine
//
//  Created by Zaid Aggad on 10/06/2019.
//  Copyright © 2019 Google. All rights reserved.
//

import UIKit

class alertcell: UITableViewCell {

    @IBOutlet weak var title: UILabel!
    @IBOutlet weak var date: UILabel!
    @IBOutlet weak var information: UILabel!
    @IBOutlet weak var Aimage: UIImageView!
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

}
